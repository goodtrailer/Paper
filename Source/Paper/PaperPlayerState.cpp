// Copyright (c) 2019–2020 Alden Wu

#include "PaperPlayerState.h"

#include "PaperGameState.h"
#include "PaperEnums.h"
#include "PaperGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

bool APaperPlayerState::IsTurn()
{
	// for whatever reason, this is super problematic (even with nullptr checks) for clients during intial connection and can throw exception access violations,
	// so here's a try catch (yucky but inevitable).

	// UPDATE: try catch block works. most of the time. miraculously, if i spam click enough, it STILL FUCKING CRASHES. WHY THE FUCK DOES THIS HAPPEN?
	// IT'S A FREAKING TRY CATCH BLOCK. HOW THE FUCK. HAS I EVER?
	try
	{
		bool b = (GetWorld()->GetGameState<APaperGameState>()->Turn % GetGameInstance<UPaperGameInstance>()->BoardInfo.SpawnNumber == static_cast<uint8>(GetGameInstance<UPaperGameInstance>()->Team)) ? true : false;
		return b;
	}
	catch (...)
	{
		return false;
	}
}

void APaperPlayerState::BeginPlay()
{
	Server_SetTeam(GetGameInstance<UPaperGameInstance>()->Team);
}

void APaperPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APaperPlayerState, Team)
}

APaperPlayerState::APaperPlayerState()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void APaperPlayerState::Server_SetTeam_Implementation(ETeam TeamToSet)
{
	Team = TeamToSet;
}

bool APaperPlayerState::Server_SetTeam_Validate(ETeam TeamToSet)
{
	return true;
}
