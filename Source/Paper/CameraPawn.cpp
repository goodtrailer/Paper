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

#define BAKED_ROTATE_SENSITIVITY 3.f
#define BAKED_PAN_SENSITIVITY 0.01f
#define BAKED_ZOOM_SENSITIVITY 0.1f;

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
		AddActorLocalRotation(FRotator(MouseDeltaY, 0.f, 0.f) * RotateSensitivity * BAKED_ROTATE_SENSITIVITY);
		AddActorWorldRotation(FRotator(0.f, MouseDeltaX, 0.f) * RotateSensitivity * BAKED_ROTATE_SENSITIVITY);
	}
	else if (bIsPanning)
		AddActorLocalOffset(FVector(0.f, MouseDeltaX, MouseDeltaY) * -PanSensitivity * SpringArm->TargetArmLength * BAKED_PAN_SENSITIVITY);
}

void ACameraPawn::ZoomIn()
{
	SpringArm->TargetArmLength -= ZoomSensitivity * SpringArm->TargetArmLength * BAKED_ZOOM_SENSITIVITY;
}

void ACameraPawn::ZoomOut()
{
	SpringArm->TargetArmLength += ZoomSensitivity * SpringArm->TargetArmLength * BAKED_ZOOM_SENSITIVITY;
}

void ACameraPawn::ResetPosition()
{
	if (APaperGameState* GS = GetWorld()->GetGameState<APaperGameState>())
		SetActorLocation(FVector(GS->GetBoardWidth() * 100.f, GS->GetBoardHeight() * 100.f, 300.f));
}
