// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPawn.h"

ACameraPawn::ACameraPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Scene);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	SpringArm->TargetArmLength = 5000;
	SpringArm->bDoCollisionTest = false;

	ZoomSensitivity = 1.f;
	RotateSensitivity = 2.5f;
	PanSensitivity = 10.f;

}

void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();
	
	FVector MyVector(2000.f, 2800.f, 300.f);
	SetActorLocation(MyVector);
	
}

void ACameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bRotateButtonDown)
	{
		FRotator Yaw(0.f, MouseX, 0.f);
		AddActorWorldRotation(Yaw * RotateSensitivity);
		FRotator Pitch(MouseY, 0.f, 0.f);
		AddActorLocalRotation(Pitch * RotateSensitivity);
	}
	else if (bPanButtonDown)
	{
		FVector MyVector(0.f, MouseX, MouseY);
		AddActorLocalOffset(MyVector * -PanSensitivity * SpringArm->TargetArmLength / 1000);
	}
}

void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Rotate Button", IE_Pressed, this, &ACameraPawn::SetTruebRotateButtonDown);
	PlayerInputComponent->BindAction("Rotate Button", IE_Released, this, &ACameraPawn::SetFalsebRotateButtonDown);
	PlayerInputComponent->BindAction("Pan Button", IE_Pressed, this, &ACameraPawn::SetTruebPanButtonDown);
	PlayerInputComponent->BindAction("Pan Button", IE_Released, this, &ACameraPawn::SetFalsebPanButtonDown);
	PlayerInputComponent->BindAction("Zoom In", IE_Pressed, this, &ACameraPawn::ZoomIn);
	PlayerInputComponent->BindAction("Zoom Out", IE_Pressed, this, &ACameraPawn::ZoomOut);
	PlayerInputComponent->BindAxis("Mouse X", this, &ACameraPawn::SetMouseX);
	PlayerInputComponent->BindAxis("Mouse Y", this, &ACameraPawn::SetMouseY);
}

void ACameraPawn::ZoomIn()
{
	SpringArm->TargetArmLength -= 100 * ZoomSensitivity * SpringArm->TargetArmLength / 1000;
	UE_LOG(LogTemp, Display, TEXT("BUMP"))
}

void ACameraPawn::ZoomOut()
{
	SpringArm->TargetArmLength += 100 * ZoomSensitivity * SpringArm->TargetArmLength / 1000;
}

void ACameraPawn::SetTruebPanButtonDown()
{
	bPanButtonDown = true;
}

void ACameraPawn::SetFalsebPanButtonDown()
{
	bPanButtonDown = false;
}

void ACameraPawn::SetTruebRotateButtonDown()
{
	bRotateButtonDown = true;
}

void ACameraPawn::SetFalsebRotateButtonDown()
{
	bRotateButtonDown = false;
}

void ACameraPawn::SetMouseX(float f)
{
	MouseX = f;
}

void ACameraPawn::SetMouseY(float f)
{
	MouseY = f;
}
