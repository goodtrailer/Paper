// Copyright (c) 2019–2020 Alden Wu

#include "Unit.h"

#include "PaperEnums.h"
#include "PaperGameInstance.h"
#include "PaperGameState.h"
#include "Net/UnrealNetwork.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = false;
	Type = EType::TypeUnit;
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
	case ERangeType::RangeTypeNormal:
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

	case ERangeType::RangeTypeSquare:
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

void AUnit::OnRep_Coordinates()
{
	SetActorLocation(FVector(Coordinates % GetGameInstance<UPaperGameInstance>()->BoardInfo.SizeX * 200, Coordinates / GetGameInstance<UPaperGameInstance>()->BoardInfo.SizeX * 200, GetActorLocation().Z));
	OnRep_RecordedStat();
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
