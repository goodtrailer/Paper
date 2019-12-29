// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPawn.h"
#include "Unit.h"
#include "GameFramework/PlayerState.h"
#include "EngineUtils.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

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
	
	for (TActorIterator<ABoardGenerator> i(GetWorld()); i; ++i)
		BoardGenerator = *i;
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



bool ACameraPawn::IsTurn()
{
	if (BoardGenerator->Turn % 2 == Team)
		return true;
	else
		return false;
}



void ACameraPawn::EndTurn()
{
	BoardGenerator->Turn++;
}

bool ACameraPawn::Server_EndTurn_Validate()
{
	return true;
}

void ACameraPawn::Server_EndTurn_Implementation()
{
	if (IsRunningDedicatedServer())
		EndTurn();
	Multicast_EndTurn();
}

void ACameraPawn::Multicast_EndTurn_Implementation()
{
	EndTurn();
}



void ACameraPawn::SpawnUnit(uint8 team, TSubclassOf<AUnit> type)
{
	if (!BoardGenerator->SpawnUnit(team, type))
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Spawns are full!"));
}

bool ACameraPawn::Server_SpawnUnit_Validate(uint8 team, TSubclassOf<AUnit> type)
{
	return true;
}

void ACameraPawn::Server_SpawnUnit_Implementation(uint8 team, TSubclassOf<AUnit> type)
{
	if (IsRunningDedicatedServer())
		SpawnUnit(team, type);
	Multicast_SpawnUnit(team, type);
}

void ACameraPawn::Multicast_SpawnUnit_Implementation(uint8 team, TSubclassOf<AUnit> type)
{
	SpawnUnit(team, type);
}



void ACameraPawn::Debug()
{
	UE_LOG(LogTemp, Display, TEXT("%d"), BoardGenerator->Turn)
	GEngine->AddOnScreenDebugMessage(1, 8.f, FColor::Yellow, TEXT("Debug!"));
	//UE_LOG(LogTemp, Display, TEXT("Team: %d\nTurn: %d\nIs Turn?: %s"), Team, Turn, (IsTurn()) ? TEXT("True") : TEXT("False?"))
}

bool ACameraPawn::Server_Debug_Validate()
{
	return true;
}

void ACameraPawn::Server_Debug_Implementation()
{
	if (IsRunningDedicatedServer())
		Debug();
	Multicast_Debug();
}

void ACameraPawn::Multicast_Debug_Implementation()
{
	Debug();
}



void ACameraPawn::Client_SetTeam_Implementation(uint8 t)
{
	Team = t;
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
	PlayerInputComponent->BindAction("Debug", IE_Pressed, this, &ACameraPawn::Debug);
}

void ACameraPawn::ZoomIn()
{
	SpringArm->TargetArmLength -= 100 * ZoomSensitivity * SpringArm->TargetArmLength / 1000;
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
