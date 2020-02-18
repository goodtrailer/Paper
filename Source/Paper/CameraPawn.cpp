// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPawn.h"
#include "Unit.h"
#include "GameFramework/PlayerController.h"
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
	if (!IsRunningDedicatedServer())
		PlayerController = Cast<APlayerController>(GetController());
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
	else if (bPanButtonDown && (SelectedUnit == nullptr || SelectedUnit->Team != Team))
	{
		FVector MyVector(0.f, MouseX, MouseY);
		AddActorLocalOffset(MyVector * -PanSensitivity * SpringArm->TargetArmLength / 1000);
	}
	
	if (PlayerController != nullptr)
	{
		FHitResult Hit;
		PlayerController->GetHitResultUnderCursor(ECC_GameTraceChannel1, false, Hit);
		HoveredUnit = Cast<AUnit>(Hit.GetActor());
	}

	if (SelectedUnit == nullptr)
		GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Red, TEXT("Selected NULL"), false);
	else
		GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Green, FString::Printf(TEXT("Selected %d"), SelectedUnit->Type), false);

	if (HoveredUnit == nullptr)
		GEngine->AddOnScreenDebugMessage(6, 1.f, FColor::Red, TEXT("Hovered NULL"), false);
	else
		GEngine->AddOnScreenDebugMessage(6, 1.f, FColor::Green, FString::Printf(TEXT("Hovered %d"), HoveredUnit->Type), false);

}



bool ACameraPawn::IsTurn()
{
	if (BoardGenerator->Turn % 2 == static_cast<uint8>(Team))
		return true;
	else
		return false;
}



void ACameraPawn::EndTurn()
{
	BoardGenerator->Turn++;
}

void ACameraPawn::Server_EndTurn_Implementation()
{
	if (IsRunningDedicatedServer())
		EndTurn();
	Multicast_EndTurn();
}

bool ACameraPawn::Server_EndTurn_Validate()
{
	return true;
}

void ACameraPawn::Multicast_EndTurn_Implementation()
{
	EndTurn();
}



void ACameraPawn::SpawnUnit(ETeam team, TSubclassOf<AUnit> type)
{
	if (!BoardGenerator->SpawnUnit(team, type))
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Spawns are full!"));
}

void ACameraPawn::Server_SpawnUnit_Implementation(ETeam team, TSubclassOf<AUnit> type)
{
	if (IsRunningDedicatedServer())
		SpawnUnit(team, type);
	Multicast_SpawnUnit(team, type);
}

bool ACameraPawn::Server_SpawnUnit_Validate(ETeam team, TSubclassOf<AUnit> type)
{
	return true;
}

void ACameraPawn::Multicast_SpawnUnit_Implementation(ETeam team, TSubclassOf<AUnit> type)
{
	SpawnUnit(team, type);
}



void ACameraPawn::Debug()
{
	UE_LOG(LogTemp, Display, TEXT("%d"), BoardGenerator->Turn)
	GEngine->AddOnScreenDebugMessage(1, 8.f, FColor::Yellow, TEXT("Debug!"));
	//UE_LOG(LogTemp, Display, TEXT("Team: %d\nTurn: %d\nIs Turn?: %s"), Team, Turn, (IsTurn()) ? TEXT("True") : TEXT("False?"))
}

void ACameraPawn::Server_Debug_Implementation()
{
	if (IsRunningDedicatedServer())
		Debug();
	Multicast_Debug();
}

bool ACameraPawn::Server_Debug_Validate()
{
	return true;
}

void ACameraPawn::Multicast_Debug_Implementation()
{
	Debug();
}



void ACameraPawn::Client_SetTeam_Implementation(ETeam t)
{
	Team = t;
}



void ACameraPawn::SelectUnit()
{
	if (HoveredUnit != nullptr && HoveredUnit->bIsTargetable)
	{
		SelectedUnit = HoveredUnit;
		bSelectButtonDown = true;
		MoveOverlayOn();
	}
	else
		SelectedUnit = nullptr;
}

void ACameraPawn::MoveUnit()
{
	bSelectButtonDown = false;
	MoveOverlayOff();
	if (BoardGenerator->Turn % 2 == static_cast<int>(Team) && SelectedUnit != nullptr && HoveredUnit != nullptr && SelectedUnit->Team == Team)
		SelectedUnit->Server_MoveTo(HoveredUnit->Coordinates, BoardGenerator->BoardWidth);
}

void ACameraPawn::MoveOverlayOn()
{
	if (BoardGenerator->Turn % 2 == static_cast<int>(Team) && SelectedUnit->Team == Team)
		GEngine->AddOnScreenDebugMessage(3, 1.f, FColor::Green, TEXT("Move Overlay ON"), false);
}

void ACameraPawn::MoveOverlayOff()
{
	if (BoardGenerator->Turn % 2 == static_cast<int>(Team) && SelectedUnit != nullptr && SelectedUnit->Team == Team)
		GEngine->AddOnScreenDebugMessage(3, 1.f, FColor::Red, TEXT("Move Overlay OFF"), false);
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
	PlayerInputComponent->BindAction("Select Unit", IE_Pressed, this, &ACameraPawn::SelectUnit);
	PlayerInputComponent->BindAction("Select Unit", IE_Released, this, &ACameraPawn::MoveUnit);
	PlayerInputComponent->BindAction("Move Unit", IE_Pressed, this, &ACameraPawn::MoveOverlayOn);
	PlayerInputComponent->BindAction("Move Unit", IE_Released, this, &ACameraPawn::MoveUnit);
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
