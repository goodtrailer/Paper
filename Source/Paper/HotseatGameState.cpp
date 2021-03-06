// Copyright (c) 2019–2020 Alden Wu

#include "HotseatGameState.h"
#include "Unit.h"
#include "PaperEnums.h"
#include "PaperPlayerState.h"
#include "PaperPlayerController.h"
#include "PaperPlayerState.h"
#include "PaperUserInterface.h"


void AHotseatGameState::BeginPlay()
{
	APaperGameState::BeginPlay();
	Pawn = GEngine->GetFirstLocalPlayerController(GetWorld())->GetPawn();
}

void AHotseatGameState::Defeat(ETeam DefeatedTeam)
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
					if (aliveCount)
						goto OutOfLoop;
					aliveCount++;
					winner = static_cast<ETeam>(i);
				}
			}
			bGameStarted = false;
			Multicast_Victory(winner);
		}

	OutOfLoop:
		;	// CODE
	}
}

void AHotseatGameState::EndTurn()
{
	APaperGameState::EndTurn();
	APaperPlayerController* PC = Cast<APaperPlayerController>(Pawn->GetController());
	APaperPlayerState* PS = PC->GetPaperPlayerState();
	ETeam NewTeam = static_cast<ETeam>(Turn % TeamCount);
	PS->SetTeam(NewTeam);
	PC->UserInterface->UpdateTeam(NewTeam);
}
