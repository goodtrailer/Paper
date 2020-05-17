// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPawn.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "EngineUtils.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

ACameraPawn::ACameraPawn()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ACameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRotating)
	{
		AddActorLocalRotation(FRotator(MouseDeltaY, 0.f, 0.f) * RotateSensitivity);
		AddActorWorldRotation(FRotator(0.f, MouseDeltaX, 0.f) * RotateSensitivity);
	}
	else if (bIsPanning)
		AddActorLocalOffset(FVector(0.f, MouseDeltaX, MouseDeltaY) * -PanSensitivity * SpringArm->TargetArmLength / 1000);
}

void ACameraPawn::ZoomIn()
{
	GLog->Log(TEXT("ACameraPawn::ZoomIn()"));
	SpringArm->TargetArmLength -= ZoomSensitivity * SpringArm->TargetArmLength / 10;
}

void ACameraPawn::ZoomOut()
{
	GLog->Log(TEXT("ACameraPawn::ZoomOut()"));
	SpringArm->TargetArmLength += ZoomSensitivity * SpringArm->TargetArmLength / 10;
}


#if 0
void ACameraPawn::EndTurn()
{
	BoardGenerator->Turn++;
	for (int i = 0; i < BoardGenerator->BoardWidth * BoardGenerator->BoardHeight; i++)
		if (BoardGenerator->UnitBoard[i] && static_cast<uint8>(BoardGenerator->UnitBoard[i]->Team) == BoardGenerator->Turn % 2)
			BoardGenerator->UnitBoard[i]->Passive();
}

void ACameraPawn::SpawnUnit(ETeam team, TSubclassOf<AUnit> type)
{
	if (!BoardGenerator->SpawnUnit(team, type))
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Spawns are full!"));
}
#endif
