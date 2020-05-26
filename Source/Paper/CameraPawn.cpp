// Fill out your copyright notice in the Description page of Project Settings.

#include "CameraPawn.h"

#include "Unit.h"
#include "Cardinal.h"
#include "GameFramework/SpringArmComponent.h"
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
	SpringArm->TargetArmLength -= ZoomSensitivity * SpringArm->TargetArmLength / 10;
}

void ACameraPawn::ZoomOut()
{
	SpringArm->TargetArmLength += ZoomSensitivity * SpringArm->TargetArmLength / 10;
}
