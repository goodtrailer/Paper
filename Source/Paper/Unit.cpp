// Copyright (c) 2019�2020 Alden Wu

#include "Unit.h"

#include "PaperEnums.h"
#include "PaperGameState.h"
#include "PaperPlayerController.h"
#include "Ground.h"
#include "TruncatedPrism.h"
#include "GlobalStatics.h"
#include "Net/UnrealNetwork.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = false;
	Type = EType::Unit;
	Team = static_cast<ETeam>(-1);				// force replication
	bIsTargetable = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	bHPPrismMeterAutoMat = true;
	SetReplicateMovement(true);
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(L"DefaultSceneRoot");
	HPPrismMeter = CreateDefaultSubobject<UTruncatedPrism>(L"HPPrismMeter");
	HPPrismMeter->SetupAttachment(RootComponent);
	HPPrismMeter->SetRelativeLocation( { 0.f, 0.f, -100.f } );

	NetUpdateFrequency = 60.f;
	MinNetUpdateFrequency = 4.f;
}

void AUnit::DetermineMovableTiles_Implementation(TMap<int, FMovableTileInfo>& OutMovableTiles) const
{
	APaperGameState* GameState = GetWorld()->GetGameState<APaperGameState>();
	TSet<int> TilesForNextPass;
	TSet<int> TilesForCurrentPass;
	TSet<int> TilesPreviouslyQueuedForPassing;

	TilesPreviouslyQueuedForPassing.Add(Coordinates);
	TilesForNextPass.Add(Coordinates);

	int UpCoord, RightCoord, DownCoord, LeftCoord;

#if 0
	// Check above tile
	if (// Make sure the tile above is within board bounds
		Coordinates / GameState->GetBoardWidth() > 0
		// Can't move to tile above if already occupied by another unit. Also, no weird maths, because we are never dealing with edge cases, due to the above condition. im so dam smart.
		&& !GameState->UnitBoard[UpIndex]
		// Prevent repeating passing on tiles, since that would result in using up all your energy for any move, even one space moves
		&& !TilesPreviouslyQueuedForPassing.Contains(UpIndex)
		// Check if ground tile exists upwards, since there could be holes
		&& GameState->GroundBoard[UpIndex]
		// Check if passable upwards, since there are ground tiles that block one direction
		&& !(GameState->GroundBoard[UpIndex]->CollidableDirections & EDirection::Down))
	{
		TilesForNextPass.Add(UpIndex);
		TilesPreviouslyQueuedForPassing.Add(UpIndex);
		OutMovableTiles.Add(UpIndex, { 0, Coordinates, EDirection::Down });
	}

	// Same calculations, but right
	if (Coordinates % GameState->GetBoardWidth() < GameState->GetBoardWidth() - 1 && !GameState->UnitBoard[RightIndex] && !TilesPreviouslyQueuedForPassing.Contains(RightIndex) && GameState->GroundBoard[RightIndex] && !(GameState->GroundBoard[RightIndex]->CollidableDirections & EDirection::Left))
	{
		TilesForNextPass.Add(RightIndex);
		TilesPreviouslyQueuedForPassing.Add(RightIndex);
		OutMovableTiles.Add(RightIndex, { 0, Coordinates, EDirection::Left });
	}

	// Same calculations, but down
	if (Coordinates / GameState->GetBoardWidth() < GameState->GetBoardHeight() - 1 && !GameState->UnitBoard[DownIndex] && !TilesPreviouslyQueuedForPassing.Contains(DownIndex) && GameState->GroundBoard[DownIndex] && !(GameState->GroundBoard[DownIndex]->CollidableDirections & EDirection::Up))
	{
		TilesForNextPass.Add(DownIndex);
		TilesPreviouslyQueuedForPassing.Add(DownIndex);
		OutMovableTiles.Add(DownIndex, { 0, Coordinates, EDirection::Up });
	}

	// Same calculations, but left
	if (Coordinates % GameState->GetBoardWidth() > 0 && !GameState->UnitBoard[LeftIndex] && !TilesPreviouslyQueuedForPassing.Contains(LeftIndex) && GameState->GroundBoard[LeftIndex] && !(GameState->GroundBoard[LeftIndex]->CollidableDirections & EDirection::Right))
	{
		TilesForNextPass.Add(LeftIndex);
		TilesPreviouslyQueuedForPassing.Add(LeftIndex);
		OutMovableTiles.Add(LeftIndex, { 0, Coordinates, EDirection::Right });
	}
#endif

	for (uint8 EnergyLeft = Energy; EnergyLeft > 0; EnergyLeft--)
	{
		TilesForCurrentPass = TilesForNextPass;
		TilesForNextPass.Empty();
		for (auto& coord : TilesForCurrentPass)
		{
			UpCoord = coord - GameState->GetBoardWidth();
			RightCoord = coord + 1;
			DownCoord = coord + GameState->GetBoardWidth();
			LeftCoord = coord - 1;
			
			// check up
			if (coord / GameState->GetBoardWidth() > 0 && !GameState->UnitBoard[UpCoord] && !TilesPreviouslyQueuedForPassing.Contains(UpCoord) && GameState->GroundBoard[UpCoord] && !(GameState->GroundBoard[UpCoord]->CollidableDirections & EDirection::Down))
			{
				TilesForNextPass.Add(UpCoord);
				TilesPreviouslyQueuedForPassing.Add(UpCoord);
				OutMovableTiles.Add(UpCoord, { (uint8)(EnergyLeft - 1), coord, EDirection::Down });
			}

			// check right
			if (coord % GameState->GetBoardWidth() < GameState->GetBoardWidth() - 1 && !GameState->UnitBoard[RightCoord] && !TilesPreviouslyQueuedForPassing.Contains(RightCoord) && GameState->GroundBoard[RightCoord] && !(GameState->GroundBoard[RightCoord]->CollidableDirections & EDirection::Left))
			{
				TilesForNextPass.Add(RightCoord);
				TilesPreviouslyQueuedForPassing.Add(RightCoord);
				OutMovableTiles.Add(RightCoord, { (uint8)(EnergyLeft - 1), coord, EDirection::Left });
			}

			// check down
			if (coord / GameState->GetBoardWidth() < GameState->GetBoardHeight() - 1 && !GameState->UnitBoard[DownCoord] && !TilesPreviouslyQueuedForPassing.Contains(DownCoord) && GameState->GroundBoard[DownCoord] && !(GameState->GroundBoard[DownCoord]->CollidableDirections & EDirection::Up))
			{
				TilesForNextPass.Add(DownCoord);
				TilesPreviouslyQueuedForPassing.Add(DownCoord);
				OutMovableTiles.Add(DownCoord, { (uint8)(EnergyLeft - 1), coord, EDirection::Up });
			}

			// check left
			if (coord % GameState->GetBoardWidth() > 0 && !GameState->UnitBoard[LeftCoord] && !TilesPreviouslyQueuedForPassing.Contains(LeftCoord) && GameState->GroundBoard[LeftCoord] && !(GameState->GroundBoard[LeftCoord]->CollidableDirections & EDirection::Right))
			{
				TilesForNextPass.Add(LeftCoord);
				TilesPreviouslyQueuedForPassing.Add(LeftCoord);
				OutMovableTiles.Add(LeftCoord, { (uint8)(EnergyLeft - 1), coord, EDirection::Right });
			}
		}
	}
}

void AUnit::DetermineAttackableTiles_Implementation(TSet<int>& OutReachableTiles, TSet<int>& OutAttackableTiles) const
{
	APaperGameState* GameState = GetWorld()->GetGameState<APaperGameState>();
	const int BoardWidth = GameState->GetBoardWidth();
	const int BoardHeight = GameState->GetBoardHeight();

	switch (RangeType)
	{
	case ERangeType::Normal:
		for (int i = 0; i <= Range; i++)
			for (int j = i - Range; j <= Range - i; j++)
				if (Coordinates / BoardWidth + j < BoardHeight && Coordinates / BoardWidth + j >= 0)
				{
					if (Coordinates % BoardWidth + i < BoardWidth)
					{
						int ReachableCoord = Coordinates + i + j * BoardWidth;
						AUnit* ReachableUnit = GameState->UnitBoard[ReachableCoord];
						OutReachableTiles.Add(ReachableCoord);
						if (ReachableUnit && ReachableUnit->Team != Team && ReachableUnit->bIsTargetable)
							OutAttackableTiles.Add(ReachableCoord);
					}
					if (Coordinates % BoardWidth - i >= 0)
					{
						int ReachableCoord = Coordinates - i + j * BoardWidth;
						AUnit* ReachableUnit = GameState->UnitBoard[ReachableCoord];
						OutReachableTiles.Add(ReachableCoord);
						OutReachableTiles.Add(ReachableCoord);
						if (ReachableUnit && ReachableUnit->Team != Team && ReachableUnit->bIsTargetable)
							OutAttackableTiles.Add(ReachableCoord);
					}
				}
		break;

	case ERangeType::Square:
		for (int i = 0; i <= Range; i++)
			for (int j = -Range; j <= Range; j++)
				if (Coordinates / BoardWidth + j < BoardHeight && Coordinates / BoardWidth + j >= 0)
				{
					if (Coordinates % BoardWidth + i < BoardWidth)
					{
						int ReachableCoord = Coordinates + i + j * BoardWidth;
						AUnit* ReachableUnit = GameState->UnitBoard[ReachableCoord];
						OutReachableTiles.Add(ReachableCoord);
						if (ReachableUnit && ReachableUnit->Team != Team && ReachableUnit->bIsTargetable)
							OutAttackableTiles.Add(ReachableCoord);
					}
					if (Coordinates % BoardWidth - i >= 0)
					{
						int ReachableCoord = Coordinates - i + j * BoardWidth;
						AUnit* ReachableUnit = GameState->UnitBoard[ReachableCoord];
						OutReachableTiles.Add(ReachableCoord);
						OutReachableTiles.Add(ReachableCoord);
						if (ReachableUnit && ReachableUnit->Team != Team && ReachableUnit->bIsTargetable)
							OutAttackableTiles.Add(ReachableCoord);
					}
				}
		break;
	}
	
}

// should only be called by server
void AUnit::Attack_Implementation(AUnit* Victim)
{
	Energy -= 2;
	if (Damage < Victim->GetHP())
		Victim->SetHP(Victim->GetHP() - Damage);
	else
	{
		APaperGameState* GameState = GetWorld()->GetGameState<APaperGameState>();
		GameState->Multicast_CheckDeadUnitForLocalPlayerController(Victim);
		Victim->Die();
		GameState->UnitBoard[Victim->Coordinates] = nullptr;
	}
}

// should only be called by server
void AUnit::Die_Implementation()
{
	Destroy();
}

// should only be called by server
void AUnit::Passive_Implementation()
{
	Energy = FGenericPlatformMath::Max(EnergyMax, Energy);
}

void AUnit::OnRep_Team()
{
	if (auto Mesh = GetMeshComponent())
		Mesh->SetMaterial(0, GetMaterial());
	if (HPPrismMeter && bHPPrismMeterAutoMat && Cast<UGlobalStatics>(GEngine->GameSingleton))
		HPPrismMeter->SetMaterial(0, Cast<UGlobalStatics>(GEngine->GameSingleton)->HPPrismMeterMaterials[static_cast<uint8>(Team)]);
}

void AUnit::OnRep_RecordedStat()
{
	if (GetWorld())
	{
		APaperGameState* const GameState = GetWorld()->GetGameState<APaperGameState>();
		if (GameState)
			GameState->CheckUpdatedUnitForLocalPlayerController(this);
	}
}

void AUnit::OnRep_HP()
{
	if (HPPrismMeter)
		HPPrismMeter->Truncate((float)HP / (float)HPMax);
	OnRep_RecordedStat();
}

int AUnit::GetCost_Implementation()
{
	return 0;
}

uint8 AUnit::GetHP() const { return HP; }

uint8 AUnit::GetHPMax() const { return HPMax; }

void AUnit::SetHP(uint8 a)
{
	HP = a;
	OnRep_HP();
}

uint8 AUnit::GetEnergyMax() const { return EnergyMax; }

FIntPoint AUnit::GetCoordinatesVector()
{
	if (APaperGameState* GS = GetWorld()->GetGameState<APaperGameState>())
		return { Coordinates % GS->GetBoardWidth(), Coordinates / GS->GetBoardWidth() };
	else
		return { -1, -1 };
}

UMeshComponent* AUnit::GetMeshComponent()
{
	return nullptr;
}

void AUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUnit, Team)
	DOREPLIFETIME(AUnit, Type)
	DOREPLIFETIME(AUnit, Damage)
	DOREPLIFETIME(AUnit, Range)
	DOREPLIFETIME(AUnit, RangeType)
	DOREPLIFETIME(AUnit, Energy)
	DOREPLIFETIME(AUnit, Coordinates)
	DOREPLIFETIME(AUnit, EnergyMax)
	DOREPLIFETIME(AUnit, HP)
	DOREPLIFETIME(AUnit, HPMax)
	DOREPLIFETIME(AUnit, PassiveString)
}