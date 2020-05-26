#include "Unit.h"

#include "PaperEnums.h"
#include "PaperGameInstance.h"
#include "PaperGameState.h"
#include "Net/UnrealNetwork.h"

void AUnit::Passive_Implementation()
{
	Energy = EnergyMax;
}

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = false;
	Type = EType::TypeUnit;
	bReplicates = true;
	bAlwaysRelevant = true;
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