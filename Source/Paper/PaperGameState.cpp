// Copyright (c) 2019–2020 Alden Wu

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
		for (auto& Unit : UnitBoard)
			if (Unit && Unit->Team == DefeatedTeam)
				Unit->Destroy();

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
		else
			GLog->Log(TEXT("Remove player failed!"));
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
	if (Turn / TeamCount > 0)
	{
		float Now = GetWorld()->GetTimeSeconds();
		float Elapsed = Now - TurnStartTime;
		TeamTimers[Turn % TeamCount] -= FMath::Max(0.f, Elapsed - CurrentDelay);
		TurnStartTime = Now;
	}
	
	do
	{
		++Turn;
	} while (TeamStatuses[Turn % TeamCount] != EStatus::Alive);

	ChangeGold(static_cast<ETeam>((Turn) % BoardSpawns.Num()), PassiveIncome);
	for (auto Unit : UnitBoard)
		if (Unit && Turn % BoardSpawns.Num() == static_cast<uint8>(Unit->Team))
			Unit->Passive();
	OnRep_Turn();
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
	DOREPLIFETIME(APaperGameState, CroppedBoardLayout)
	DOREPLIFETIME(APaperGameState, bGameStarted)
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

void APaperGameState::OnRep_Turn()
{
	if (APaperPlayerController* LocalPC = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())))
		if (LocalPC->bInGame)
			LocalPC->UserInterface->UpdateTurn(Turn % TeamCount == static_cast<uint8>(LocalPC->GetPaperPlayerState()->Team));
}

void APaperGameState::OnRep_CroppedBoardLayout()
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

void APaperGameState::OnRep_TeamTimers()
{
	if (APaperPlayerController* LocalPC = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())))
		if (LocalPC->bInGame)
		{

		}
}

void APaperGameState::OnRep_InitialTimer()
{
	GLog->Log(L"OnRep_InitialTimer");
	if (APaperPlayerController* LocalPC = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())))
		if (ULobbyUserInterface* LobbyUI = LocalPC->LobbyInterface)
			if (USpinBox* TimerUI = LobbyUI->TimerInterface)
				TimerUI->SetValue(InitialTimer);
}

void APaperGameState::OnRep_DelayCoefficient()
{
	GLog->Log(L"OnRep_DelayCoefficient");
	if (APaperPlayerController* LocalPC = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())))
		if (ULobbyUserInterface* LobbyUI = LocalPC->LobbyInterface)
			if (USpinBox* DelayCoefficientUI = LobbyUI->DelayCoefficientInterface)
				DelayCoefficientUI->SetValue(DelayCoefficient);
}
