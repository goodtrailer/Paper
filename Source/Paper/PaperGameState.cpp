// Copyright (c) 2019�2020 Alden Wu

#include "PaperGameState.h"

#include "Unit.h"
#include "PaperEnums.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "LobbyUserInterface.h"
#include "ChatUserInterface.h"
#include "BoardPreviewUserInterface.h"
#include "PaperPlayerController.h"
#include "PaperUserInterface.h"
#include "PaperPlayerState.h"
#include "Components/SpinBox.h"
#include "Kismet/GameplayStatics.h"
#include "GlobalStatics.h"

#define TIMER_DELAY_MIN 15

AUnit* APaperGameState::GetUnit(const FIntPoint& CoordinatesVector)
{
	if (CoordinatesVector.X < 0
		|| CoordinatesVector.X >= BoardWidth
		|| CoordinatesVector.Y < 0
		|| CoordinatesVector.Y >= BoardHeight)
		return nullptr;

	return UnitBoard[CoordinatesVector.X + CoordinatesVector.Y * BoardWidth];

}

void APaperGameState::Defeat(ETeam DefeatedTeam)
{
	if (static_cast<uint8>(DefeatedTeam) < TeamCount)
	{
		// destroys the team's units
		for (int i = 0; i < UnitBoard.Num(); i++)
		{
			if (UnitBoard[i] && UnitBoard[i]->Team == DefeatedTeam)
				UnitBoard[i]->Destroy();
			UnitBoard[i] = nullptr;
		}

		// destroys the team's spawns
		for (auto& Spawn : BoardSpawns[static_cast<uint8>(DefeatedTeam)].Spawns)
			Spawn->Destroy();
		BoardSpawns[static_cast<uint8>(DefeatedTeam)].Spawns.Empty();

		TeamStatuses[static_cast<uint8>(DefeatedTeam)] = EStatus::Dead;
		{
			int aliveCount = 0;
			ETeam winner = static_cast<ETeam>(-1);	// same as 255 because uint8
			for (int i = 0; i < TeamStatuses.Num(); i++)
			{
				if (TeamStatuses[i] == EStatus::Alive)
				{
					if (aliveCount)					// one other alive team already counted, so it's now known that multiple teams are still alive.
					{
						Multicast_Defeat(DefeatedTeam);
						goto OutOfLoop;				// forgive me for the goto, it's so we skip multicast_victory. i could make an extra bool + if, but thats spaghetti af :( an alternative is an early return, and making sure all other code goes above, but frick that, im feeling rebellious
					}
					aliveCount++;
					winner = static_cast<ETeam>(i);
				}
			}
			bGameStarted = false;
			Multicast_Victory(winner);				// if we get here, then no multicast_defeat, meaning either at most one player is alive, so broadcast victory screen to that living player. if there is none, the victory goes to team -1, which doesn't exist (hopefully ?).
		}

	OutOfLoop:
		;	// CODE
	}
}

void APaperGameState::Multicast_Victory_Implementation(ETeam Team)
{
	if (APaperPlayerController* LocalPlayerController = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())))
		LocalPlayerController->CheckVictory(Team);
}

void APaperGameState::Multicast_Defeat_Implementation(ETeam Team)
{
	if (APaperPlayerController* LocalPlayerController = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())))
		LocalPlayerController->CheckDefeat(Team);
}

void APaperGameState::Multicast_Message_Implementation(const FText& Message)
{
	if (APaperPlayerController* PC = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())))
		if (PC->ChatInterface)
			PC->ChatInterface->AddMessage(Message);
}

void APaperGameState::Multicast_CheckDeadUnitForLocalPlayerController_Implementation(AUnit* Unit)
{
	APaperPlayerController* LocalPlayerController = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (LocalPlayerController)
		LocalPlayerController->CheckDeadUnit(Unit);
}

void APaperGameState::Multicast_StartGameForLocalPlayerController_Implementation()
{
	if (APaperPlayerController* LocalPC = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())))
		LocalPC->StartGame();
}

void APaperGameState::Multicast_RemovePlayerForLocalLobbyUI_Implementation(const FString& Name)
{
	if (APaperPlayerController* LocalPC = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())))
		if (ULobbyUserInterface* UI = LocalPC->LobbyInterface)
			UI->RemoveLobbySlot(Name);
}

void APaperGameState::Multicast_PlaySound_Implementation(USoundBase* Sound)
{
	UGameplayStatics::PlaySound2D(GetWorld(), Sound);
}

void APaperGameState::Multicast_UpdateBoardPreview_Implementation(const TArray<FColor>& CroppedBoardLayout)
{
	if (APaperPlayerController* LocalPC = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())))
	{
		if (ULobbyUserInterface* LobbyUI = LocalPC->LobbyInterface)
			if (UBoardPreviewUserInterface* BoardPreviewUI = LobbyUI->BoardPreviewInterface)
			{
				UTexture2D* BoardPreviewTexture = UTexture2D::CreateTransient(BoardWidth, BoardHeight);
				FColor* BoardPreviewMip = reinterpret_cast<FColor*>(BoardPreviewTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
				FMemory::Memcpy(BoardPreviewMip, CroppedBoardLayout.GetData(), BoardWidth * BoardHeight * sizeof(FColor));
				BoardPreviewTexture->PlatformData->Mips[0].BulkData.Unlock();
				BoardPreviewMip = nullptr;
				BoardPreviewTexture->Filter = TF_Nearest;
				BoardPreviewTexture->UpdateResource();
				BoardPreviewUI->UpdateBoardPreview(BoardPreviewTexture);
			}
		LocalPC->ResetCameraPosition();
	}
}

float APaperGameState::GetTimer(ETeam Team) const
{
	if (static_cast<int>(Team) < TeamTimers.Num())
		return TeamTimers[static_cast<int>(Team)];
	else
		return -1.f;
}

int APaperGameState::GetGold(ETeam Team) const
{
	if (static_cast<int>(Team) < Gold.Num())
		return Gold[static_cast<int>(Team)];
	else
		return -1;
}

void APaperGameState::SetGold(ETeam Team, int NewAmount)
{
	if (static_cast<int>(Team) < Gold.Num())
		Gold[static_cast<int>(Team)] = NewAmount;
	else
		GLog->Logf(TEXT("Attempted to set gold for team %d; invalid operation!"), Team);
}

void APaperGameState::ChangeGold(ETeam Team, int DeltaGold)
{
	if (static_cast<int>(Team) < Gold.Num())
	{
		Gold[static_cast<int>(Team)] += DeltaGold;
		if (DeltaGold > 0)
			TeamStats[static_cast<int>(Team)].GDP += DeltaGold;
	}
	else
		GLog->Logf(TEXT("Attempted to change gold for team %d; invalid operation!"), Team);
}

float APaperGameState::GetRemainingDelay() const
{
	float ClampedDelay = FMath::Max(CurrentDelay, TIMER_DELAY_MIN);
	if (Turn / TeamCount > 0)
	{
		float Elapsed = GetServerWorldTimeSeconds() - TurnStartTime;
		return ClampedDelay - Elapsed;
	}
	else
		return ClampedDelay;
}

int APaperGameState::GetBoardHeight() const
{
	return BoardHeight;
}

int APaperGameState::GetBoardWidth() const
{
	return BoardWidth;
}

void APaperGameState::EndTurn()
{
	if (GetRemainingDelay() < 0)
		TeamTimers[Turn % TeamCount] += GetRemainingDelay();
	TurnStartTime = GetServerWorldTimeSeconds();
	
	do
		++Turn;
	while (TeamStatuses[Turn % TeamCount] != EStatus::Alive);

	ChangeGold(static_cast<ETeam>(Turn % TeamCount), PassiveIncome);
	CurrentDelay = 0;
	
	for (auto Unit : UnitBoard)
		if (Unit && static_cast<ETeam>(Turn % TeamCount) == Unit->Team)
		{
			Unit->Passive();
			if (Unit->Type != EType::Castle)
				CurrentDelay += DelayCoefficient;
			else
				CurrentTurnCastle = Unit;
		}
	
	if (Turn / TeamCount > 0)
	{
		auto& TimerManager = GetWorldTimerManager();
		TimerManager.SetTimer(TurnTimerHandle, [&](void) {
			if (CurrentTurnCastle && bGameStarted)
				CurrentTurnCastle->Die();				// a very VERY janky way of defeating a player. this func BP overriden.
		}, GetRemainingDelay() + TeamTimers[Turn % TeamCount], false);
	}

	auto* GlobalStatics = Cast<UGlobalStatics>(GEngine->GameSingleton);
	Multicast_PlaySound(GlobalStatics->EndTurnSound);
}

void APaperGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APaperGameState, BoardSpawns)
	DOREPLIFETIME(APaperGameState, Turn)
	DOREPLIFETIME(APaperGameState, UnitBoard)
	DOREPLIFETIME(APaperGameState, GroundBoard)
	DOREPLIFETIME(APaperGameState, BoardWidth)
	DOREPLIFETIME(APaperGameState, BoardHeight)
	DOREPLIFETIME(APaperGameState, Gold)
	DOREPLIFETIME(APaperGameState, PassiveIncome)
	DOREPLIFETIME(APaperGameState, CastleHP)
	DOREPLIFETIME(APaperGameState, CastleHPMax)
	DOREPLIFETIME(APaperGameState, TeamStatuses)
	DOREPLIFETIME(APaperGameState, TeamStats)
	DOREPLIFETIME(APaperGameState, TeamTimers)
	DOREPLIFETIME(APaperGameState, TeamCount)
	DOREPLIFETIME(APaperGameState, InitialTimer)
	DOREPLIFETIME(APaperGameState, DelayCoefficient)
	DOREPLIFETIME(APaperGameState, bGameStarted)
	DOREPLIFETIME(APaperGameState, TurnStartTime)
	DOREPLIFETIME(APaperGameState, CurrentDelay)
}

APaperGameState::APaperGameState()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

AUnit* APaperGameState::GetBoardSpawn(ETeam team, int index) const
{
	return BoardSpawns[static_cast<int>(team)].Spawns[index];
}

void APaperGameState::OnRep_InitialTimer()
{
	if (APaperPlayerController* LocalPC = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())))
		if (ULobbyUserInterface* LobbyUI = LocalPC->LobbyInterface)
			if (USpinBox* TimerUI = LobbyUI->TimerInterface)
				TimerUI->SetValue(InitialTimer);
}

void APaperGameState::OnRep_DelayCoefficient()
{
	if (APaperPlayerController* LocalPC = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())))
		if (ULobbyUserInterface* LobbyUI = LocalPC->LobbyInterface)
			if (USpinBox* DelayCoefficientUI = LobbyUI->DelayCoefficientInterface)
				DelayCoefficientUI->SetValue(DelayCoefficient);
}

void APaperGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);
}
