// Copyright (c) 2019–2020 Alden Wu

#include "PaperGameState.h"

#include "Unit.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "PaperPlayerController.h"
#include "PaperUserInterface.h"
#include "PaperPlayerState.h"
#include "PaperGameInstance.h"

bool APaperGameState::Server_Defeat_Validate(ETeam DefeatedTeam)
{
	return (DefeatedTeam == ETeam::TeamNeutral) ? false : true;
}

void APaperGameState::Server_Defeat_Implementation(ETeam DefeatedTeam)
{
	if (static_cast<uint8>(DefeatedTeam) < BoardSpawns.Num())
	{
		// destroys the team's units
		for (auto& Unit : UnitBoard)
			if (Unit && Unit->Team == DefeatedTeam)
				Unit->Destroy();

		// destroys the team's spawns
		for (auto& Spawn : BoardSpawns[static_cast<uint8>(DefeatedTeam)].Spawns)
			Spawn->Destroy();
		BoardSpawns[static_cast<uint8>(DefeatedTeam)].Spawns.Empty();

		TeamStatuses[static_cast<uint8>(DefeatedTeam)] = false;
		{
			int aliveCount = 0;
			ETeam winner = static_cast<ETeam>(-1);	// same as 255 because uint8
			for (int i = 0; i < TeamStatuses.Num(); i++)
			{
				if (TeamStatuses[i])
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

void APaperGameState::Multicast_StartGame_Implementation()
{
	if (APaperPlayerController* LocalPlayerController = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())))
		LocalPlayerController->StartGame();
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

void APaperGameState::Multicast_CheckDeadUnitForLocalPlayerController_Implementation(AUnit* Unit)
{
	APaperPlayerController* LocalPlayerController = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (LocalPlayerController)
		LocalPlayerController->CheckUpdatedUnit(Unit, true);
}

void APaperGameState::CheckUpdatedUnitForLocalPlayerController(AUnit* Unit)
{
	APaperPlayerController* LocalPlayerController = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (LocalPlayerController)
		LocalPlayerController->CheckUpdatedUnit(Unit);
}

int APaperGameState::GetGold(ETeam Team) const
{
	if (static_cast<int>(Team) < Gold.Num())
		return Gold[static_cast<int>(Team)];
	else
		return -1;
}

void APaperGameState::Server_SetGold_Implementation(ETeam Team, int NewAmount)
{
	Gold[static_cast<int>(Team)] = NewAmount;
	OnRep_Gold();
}

bool APaperGameState::Server_SetGold_Validate(ETeam Team, int NewAmount)
{
	if (static_cast<int>(Team) < Gold.Num())
		return true;
	else
		return false;
}

void APaperGameState::Server_ChangeGold_Implementation(ETeam Team, int DeltaGold)
{
	Gold[static_cast<int>(Team)] += DeltaGold;
	OnRep_Gold();
}

bool APaperGameState::Server_ChangeGold_Validate(ETeam Team, int DeltaGold)
{
	if (static_cast<int>(Team) < Gold.Num())
		return true;
	else
		return false;
}

void APaperGameState::OnRep_Gold()
{
	APaperPlayerController* LocalPlayerController = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (LocalPlayerController)
	{
		
		ETeam Team = LocalPlayerController->GetPaperPlayerState()->Team;		// guaranteed to work because if local player controller exists, the so does a properly initialized game instance
		if (Team == ETeam::TeamNeutral)
		{
			// TODO: In spectator mode, update both player's gold amounts in the display, since spectators should be omniscient
		}
		else if (static_cast<int>(Team) < Gold.Num())
			LocalPlayerController->UserInterface->UpdateGold(Gold[static_cast<int>(Team)]);
	}
}

void APaperGameState::BeginPlay()
{
	Super::BeginPlay();
	//Multicast_UpdateGoldDisplay();
}

int APaperGameState::GetBoardHeight() const
{
	return BoardHeight;
}

int APaperGameState::GetBoardWidth() const
{
	return BoardWidth;
}

void APaperGameState::Server_EndTurn_Implementation()
{
	// while (!AliveTeams[++Turn % BoardSpawns.Num()]) {}; not quite as readable so i'm replacing it with do/while
	do
	{
		++Turn;
	} while (!TeamStatuses[Turn % BoardSpawns.Num()]);

	Server_ChangeGold(static_cast<ETeam>((Turn) % BoardSpawns.Num()), PassiveIncome);
	for (auto Unit : UnitBoard)
		if (Unit && Turn % BoardSpawns.Num() == static_cast<uint8>(Unit->Team))
			Unit->Passive();
	OnRep_Turn();
}

bool APaperGameState::Server_EndTurn_Validate()
{
	return true;
}

void APaperGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APaperGameState, BoardSpawns);
	DOREPLIFETIME(APaperGameState, Turn);
	DOREPLIFETIME(APaperGameState, UnitBoard);
	DOREPLIFETIME(APaperGameState, GroundBoard);
	DOREPLIFETIME(APaperGameState, BoardWidth);
	DOREPLIFETIME(APaperGameState, BoardHeight);
	DOREPLIFETIME(APaperGameState, Gold);
	DOREPLIFETIME(APaperGameState, PassiveIncome)
	DOREPLIFETIME(APaperGameState, CastleHP)
	DOREPLIFETIME(APaperGameState, CastleHPMax)
	DOREPLIFETIME(APaperGameState, TeamStatuses)
	DOREPLIFETIME(APaperGameState, TeamCount)
	DOREPLIFETIME(APaperGameState, CroppedBoardLayout)
}

APaperGameState::APaperGameState()
{
	// PrimaryActorTick.bCanEverTick = false;
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
		LocalPC->UserInterface->UpdateTurn(Turn % TeamCount == static_cast<uint8>(LocalPC->GetPaperPlayerState()->Team));
}
