// Copyright (c) 2019–2020 Alden Wu

#include "CameraPawn.h"

#include "Unit.h"
#include "PaperGameState.h"
#include "Engine/World.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "EngineUtils.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Camera/CameraComponent.h"

ACameraPawn::ACameraPawn()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();
	ResetPosition();
	SetActorRotation(FRotator(-90.f, -90.f, 0.f));
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

void ACameraPawn::ResetPosition()
{
	if (APaperGameState* GS = GetWorld()->GetGameState<APaperGameState>())
		SetActorLocation(FVector(GS->GetBoardWidth() * 100.f, GS->GetBoardHeight() * 100.f, 300.f));
}
