// Fill out your copyright notice in the Description page of Project Settings.

#include "PaperGameState.h"

#include "Unit.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "PaperPlayerController.h"
#include "PaperUserInterface.h"
#include "PaperPlayerState.h"
#include "PaperGameInstance.h"

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
		ETeam Team = GetGameInstance<UPaperGameInstance>()->Team;			// guaranteed to work because if local player controller exists, the so does a properly initialized game instance
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
	Server_ChangeGold(static_cast<ETeam>((++Turn) % BoardSpawns.Num()), PassiveIncome);
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
	APaperPlayerController* LocalPlayerController = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (LocalPlayerController)
		LocalPlayerController->UserInterface->UpdateTurn(Turn % GetGameInstance<UPaperGameInstance>()->BoardInfo.SpawnNumber == static_cast<uint8>(GetGameInstance<UPaperGameInstance>()->Team));
}
