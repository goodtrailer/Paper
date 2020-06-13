#include "Unit.h"

#include "PaperEnums.h"
#include "PaperGameInstance.h"
#include "PaperGameState.h"
#include "Net/UnrealNetwork.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Engine/Engine.h"

void AUnit::Passive_Implementation()
{
	Energy = FGenericPlatformMath::Max(EnergyMax, Energy);
}

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = false;
	Type = EType::TypeUnit;
	bReplicates = true;
	bAlwaysRelevant = true;
}

int AUnit::GetCost_Implementation()
{
	return 0;
}

void AUnit::BeginPlay()
{
	Super::BeginPlay();
	//SetOwner(GEngine->GetFirstLocalPlayerController(GetWorld()));
}

void AUnit::DetermineAttackableTiles(TSet<int>& OutReachableTiles, TSet<int>& OutAttackableTiles) const
{
	APaperGameState* GameState = GetWorld()->GetGameState<APaperGameState>();
	const int BoardWidth = GameState->GetBoardWidth();
	const int BoardHeight = GameState->GetBoardHeight();

	switch (RangeType)
	{
	case ERangeType::RangeTypeNormal:
		GLog->Log(TEXT("RangeTypeNormal"));
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
		GLog->Log(TEXT("RangeTypeSquare"));
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

bool AUnit::Server_Attack_Validate(AUnit* UnitToAttack)
{
	return true;
}

void AUnit::Server_Attack_Implementation(AUnit* UnitToAttack)
{
	if (Attack >= UnitToAttack->GetHP())
	{
		GLog->Log(TEXT("Killed"));
		UnitToAttack->Destroy();
	}
	UnitToAttack->SetHP(UnitToAttack->GetHP()- Attack);
}

void AUnit::SetHP(uint8 a)
{
	HP = a;
}

uint8 AUnit::GetHP() const { return HP; }

uint8 AUnit::GetHPMax() const { return HPMax; }

void AUnit::OnRep_Team()
{
	if (UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(GetRootComponent()->GetChildComponent(0)))
		Mesh->SetMaterial(0, TeamMaterials[static_cast<int>(Team)]);
}

void AUnit::OnRep_Coordinates()
{
	SetActorLocation(FVector(Coordinates % GetGameInstance<UPaperGameInstance>()->BoardInfo.SizeX * 200, Coordinates / GetGameInstance<UPaperGameInstance>()->BoardInfo.SizeX * 200, GetActorLocation().Z));
}

void AUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUnit, Team)
	DOREPLIFETIME(AUnit, bIsCollidable)
	DOREPLIFETIME(AUnit, bIsTargetable)
	DOREPLIFETIME(AUnit, Type)
	DOREPLIFETIME(AUnit, Attack)
	DOREPLIFETIME(AUnit, Range)
	DOREPLIFETIME(AUnit, RangeType)
	DOREPLIFETIME(AUnit, Energy)
	DOREPLIFETIME(AUnit, Coordinates)
	DOREPLIFETIME(AUnit, EnergyMax)
	DOREPLIFETIME(AUnit, HP)
	DOREPLIFETIME(AUnit, HPMax)
}
