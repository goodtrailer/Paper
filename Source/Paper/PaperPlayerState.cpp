// Copyright (c) 2019–2020 Alden Wu

#include "PaperPlayerState.h"

#include "PaperPlayerController.h"
#include "LobbyUserInterface.h"
#include "PaperGameState.h"
#include "PaperEnums.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

bool APaperPlayerState::IsTurn()
{
	// for whatever reason, this is super problematic (even with nullptr checks) for clients during intial connection and can throw exception access violations,
	// so here's a try catch (yucky but inevitable).

	// UPDATE: try catch block works. most of the time. miraculously, if i spam click enough, it STILL FUCKING CRASHES. WHY THE FUCK DOES THIS HAPPEN?
	// IT'S A FREAKING TRY CATCH BLOCK. HOW THE FUCK. HAS I EVER?

	// UPDATE 2: screw try catch blocks. they suck. i hate them. i'm traumatized by the horrors of this very simple looking method. danger lies ahead.
	
	// UPDATE 3: figured it out. ue4 stack trace was wrong. it kept saying it was throwing on the line below, but not actually. what was actually going wrong was that playerstate doesn't even exist. which is weird, because then it should be throwing on the line that you try to call this member function, but that's ue4 for you.
	if (UWorld* W = GetWorld())
		if (APaperGameState* GS = W->GetGameState<APaperGameState>())
			if (GS->TeamCount == 0)
				return false;
			else
				return (GS->Turn % GS->TeamCount == static_cast<uint8>(Team)) ? true : false;
	return false;
}

void APaperPlayerState::SetTeam(ETeam NewTeam)
{
	Team = NewTeam;
	OnRep_Team();
}

void APaperPlayerState::SetName(const FString& NewName)
{
	Name = NewName;
	OnRep_Name();
}

void APaperPlayerState::OnRep_Team()
{
	if (APaperPlayerController* PC = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())))
		if (ULobbyUserInterface* UI = PC->LobbyInterface)
			UI->UpdatePlayerTeam(Name, Team);
}

void APaperPlayerState::OnRep_Name()
{
	if (APaperPlayerController* PC = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())))
		if (ULobbyUserInterface* UI = PC->LobbyInterface)
			UI->UpdatePlayerTeam(Name, Team);
}

void APaperPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APaperPlayerState, Team)
	DOREPLIFETIME(APaperPlayerState, Name)
}

APaperPlayerState::APaperPlayerState()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	Team = ETeam::Neutral;
}
