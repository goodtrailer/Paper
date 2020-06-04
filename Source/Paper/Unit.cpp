#include "Unit.h"

#include "PaperEnums.h"
#include "PaperGameInstance.h"
#include "PaperGameState.h"
#include "Net/UnrealNetwork.h"
#include "GenericPlatform/GenericPlatformMath.h"

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

TSet<int> AUnit::DetermineAttackableTiles() const
{
	const int BoardWidth = GetWorld()->GetGameState<APaperGameState>()->GetBoardWidth();
	const int BoardHeight = GetWorld()->GetGameState<APaperGameState>()->GetBoardHeight();
	TSet<int> AttackableTiles;

	for (int i = 0; i <= Range; i++)
		for (int j = i - Range; j <= Range - i; j++)
			if (Coordinates / BoardWidth + j < BoardHeight && Coordinates / BoardWidth + j >= 0)
			{
				if (Coordinates % BoardWidth + i < BoardWidth)
					AttackableTiles.Add(Coordinates + i + j * BoardWidth);
				if (Coordinates % BoardWidth - i >= 0)
					AttackableTiles.Add(Coordinates - i + j * BoardWidth);
			}

	return AttackableTiles;
}

void AUnit::SetHP(uint8 a) { HP = a; }

uint8 AUnit::GetHP() { return HP; }

uint8 AUnit::GetHPMax() { return HPMax; }

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
