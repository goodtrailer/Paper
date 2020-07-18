// Copyright (c) 2019–2020 Alden Wu

#include "Unit.h"

#include "PaperEnums.h"
#include "PaperGameState.h"
#include "PaperPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = false;
	Type = EType::Unit;
	Team = static_cast<ETeam>(-1);				// force replication
	bReplicates = true;
	bAlwaysRelevant = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetRelativeScale3D(FVector::OneVector * 100);
	StaticMeshComponent->SetRelativeRotation(FRotator(0.f, 0.f, 90.f));

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StaticMeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);


	NetUpdateFrequency = 60.f;
	MinNetUpdateFrequency = 4.f;
}

void AUnit::DetermineMovableTiles_Implementation(TMap<int, FMovableTileInfo>& OutMovableTiles) const
{
	APaperGameState* GameState = GetWorld()->GetGameState<APaperGameState>();
	TSet<int> TilesForNextPass;
	TSet<int> TilesForCurrentPass;
	TSet<int> TilesPreviouslyQueuedForPassing;

	// Check above tile
	if (// Make sure the tile above is within board bounds
		Coordinates / GameState->GetBoardWidth() > 0
		// Can't move to tile above if already occupied by another unit. Also, no weird maths, because we are never dealing with edge cases, due to the above condition. im so dam smart.
		&& !GameState->UnitBoard[Coordinates - GameState->GetBoardWidth()]
		// Prevent repeating passing on tiles, since that would result in using up all your energy for any move, even one space moves
		&& !TilesPreviouslyQueuedForPassing.Contains(Coordinates - GameState->GetBoardWidth())
		// Check if ground tile exists upwards, since there could be holes
		&& GameState->GroundBoard[Coordinates - GameState->GetBoardWidth()]
		// Check if passable upwards, since there are ground tiles that block one direction
		&& !GameState->GroundBoard[Coordinates - GameState->GetBoardWidth()]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Down)])
	{
		TilesForNextPass.Add(Coordinates - GameState->GetBoardWidth());
		TilesPreviouslyQueuedForPassing.Add(Coordinates - GameState->GetBoardWidth());
		OutMovableTiles.Add(Coordinates - GameState->GetBoardWidth(), { 0, Coordinates, EDirection::Down });
	}

	// Same calculations, but right
	if (Coordinates % GameState->GetBoardWidth() < GameState->GetBoardWidth() - 1 && !GameState->UnitBoard[Coordinates + 1] && !TilesPreviouslyQueuedForPassing.Contains(Coordinates + 1) && GameState->GroundBoard[Coordinates + 1] && !GameState->GroundBoard[Coordinates + 1]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Left)])
	{
		TilesForNextPass.Add(Coordinates + 1);
		TilesPreviouslyQueuedForPassing.Add(Coordinates + 1);
		OutMovableTiles.Add(Coordinates + 1, { 0, Coordinates, EDirection::Left });
	}

	// Same calculations, but down
	if (Coordinates / GameState->GetBoardWidth() < GameState->GetBoardHeight() - 1 && !GameState->UnitBoard[Coordinates + GameState->GetBoardWidth()] && !TilesPreviouslyQueuedForPassing.Contains(Coordinates + GameState->GetBoardWidth()) && GameState->GroundBoard[Coordinates + GameState->GetBoardWidth()] && !GameState->GroundBoard[Coordinates + GameState->GetBoardWidth()]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Up)])
	{
		TilesForNextPass.Add(Coordinates + GameState->GetBoardWidth());
		TilesPreviouslyQueuedForPassing.Add(Coordinates + GameState->GetBoardWidth());
		OutMovableTiles.Add(Coordinates + GameState->GetBoardWidth(), { 0, Coordinates, EDirection::Up });
	}

	// Same calculations, but left
	if (Coordinates % GameState->GetBoardWidth() > 0 && !GameState->UnitBoard[Coordinates - 1] && !TilesPreviouslyQueuedForPassing.Contains(Coordinates - 1) && GameState->GroundBoard[Coordinates - 1] && !GameState->GroundBoard[Coordinates - 1]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Right)])
	{
		TilesForNextPass.Add(Coordinates - 1);
		TilesPreviouslyQueuedForPassing.Add(Coordinates - 1);
		OutMovableTiles.Add(Coordinates - 1, { 0, Coordinates, EDirection::Right });
	}

	for (int16 EnergyLeft = Energy - 1; EnergyLeft > 0; EnergyLeft--)
	{
		TilesForCurrentPass = TilesForNextPass;
		TilesForNextPass.Empty();
		for (auto& coord : TilesForCurrentPass)
		{
			OutMovableTiles[coord].EnergyLeft = EnergyLeft;
			if (coord / GameState->GetBoardWidth() > 0 && !GameState->UnitBoard[coord - GameState->GetBoardWidth()] && !TilesPreviouslyQueuedForPassing.Contains(coord - GameState->GetBoardWidth()) && GameState->GroundBoard[coord - GameState->GetBoardWidth()] && !GameState->GroundBoard[coord - GameState->GetBoardWidth()]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Down)])
			{
				TilesForNextPass.Add(coord - GameState->GetBoardWidth());
				TilesPreviouslyQueuedForPassing.Add(coord - GameState->GetBoardWidth());
				OutMovableTiles.Add(coord - GameState->GetBoardWidth(), { 0, coord, EDirection::Down });
			}

			if (coord % GameState->GetBoardWidth() < GameState->GetBoardWidth() - 1 && !GameState->UnitBoard[coord + 1] && !TilesPreviouslyQueuedForPassing.Contains(coord + 1) && GameState->GroundBoard[coord + 1] && !GameState->GroundBoard[coord + 1]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Left)])
			{
				TilesForNextPass.Add(coord + 1);
				TilesPreviouslyQueuedForPassing.Add(coord + 1);
				OutMovableTiles.Add(coord + 1, { 0, coord, EDirection::Left });
			}

			if (coord / GameState->GetBoardWidth() < GameState->GetBoardHeight() - 1 && !GameState->UnitBoard[coord + GameState->GetBoardWidth()] && !TilesPreviouslyQueuedForPassing.Contains(coord + GameState->GetBoardWidth()) && GameState->GroundBoard[coord + GameState->GetBoardWidth()] && !GameState->GroundBoard[coord + GameState->GetBoardWidth()]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Up)])
			{
				TilesForNextPass.Add(coord + GameState->GetBoardWidth());
				TilesPreviouslyQueuedForPassing.Add(coord + GameState->GetBoardWidth());
				OutMovableTiles.Add(coord + GameState->GetBoardWidth(), { 0, coord, EDirection::Up });
			}

			if (coord % GameState->GetBoardWidth() > 0 && !GameState->UnitBoard[coord - 1] && !TilesPreviouslyQueuedForPassing.Contains(coord - 1) && GameState->GroundBoard[coord - 1] && !GameState->GroundBoard[coord - 1]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Right)])
			{
				TilesForNextPass.Add(coord - 1);
				TilesPreviouslyQueuedForPassing.Add(coord - 1);
				OutMovableTiles.Add(coord - 1, { 0, coord, EDirection::Right });
			}
		}
	}
}

void AUnit::Build_Implementation(ETeam DesiredTeam)
{
	BuildMisc(true, FCardinal(true, true, true, true), DesiredTeam);
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

void AUnit::BuildMisc(bool bTargetable, FCardinal bCollidable, ETeam DesiredTeam)
{
	bIsTargetable = bTargetable;
	bIsCollidable = bCollidable;
	Team = DesiredTeam;
	StaticMeshComponent->SetMaterial(0, GetMaterial());
}

void AUnit::OnRep_Team()
{
	if (UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(GetRootComponent()->GetChildComponent(0)))
		Mesh->SetMaterial(0, GetMaterial());
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

int AUnit::GetCost_Implementation()
{
	return 0;
}

uint8 AUnit::GetHP() const { return HP; }

uint8 AUnit::GetHPMax() const { return HPMax; }

void AUnit::SetHP(uint8 a)
{
	HP = a;
}

uint8 AUnit::GetEnergyMax() const { return EnergyMax; }

void AUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUnit, Team)
	DOREPLIFETIME(AUnit, bIsCollidable)
	DOREPLIFETIME(AUnit, bIsTargetable)
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

bool AUnit::IsReplicationPausedForConnection(const FNetViewer& ConnectionOwnerNetViewer)
{
	return false;
	/*if (APaperPlayerController* PC = Cast<APaperPlayerController>(ConnectionOwnerNetViewer.InViewer))
	{
		if (PC->bInGame)
		{
			return false;
		}
	}
	return true;*/
}
