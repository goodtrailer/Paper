// Fill out your copyright notice in the Description page of Project Settings.

#include "PaperGameState.h"

#include "Unit.h"
#include "Net/UnrealNetwork.h"

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
	Turn++;
	for (auto Unit : UnitBoard)
		if (Unit && static_cast<uint8>(Unit->Team) % Turn == 0)
			Unit->Passive();
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