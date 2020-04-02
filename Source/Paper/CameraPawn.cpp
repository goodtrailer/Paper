// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPawn.h"
#include "Unit.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

ACameraPawn::ACameraPawn()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();

	// Don't wanna hard code this, but not really possible not to hard code until we get board editing, since
	// then we could be passed BoardWidth/BoardHeight before BeginPlay, so that we don't just keep getting None and errors
	SetActorLocation(FVector(2000.f, 2800.f, 300.f));

	if (!IsRunningDedicatedServer())
		PlayerController = Cast<APlayerController>(GetController());
	for (TActorIterator<ABoardGenerator> i(GetWorld()); i; ++i)
		BoardGenerator = *i;

	SelectOverlay = GetWorld()->SpawnActor<AActor>(SelectOverlayBP, FVector(0, 0, 200), FRotator::ZeroRotator);
	HoverOverlay = GetWorld()->SpawnActor<AActor>(HoverOverlayBP, FVector(0.f, 0.f, 200.f), FRotator::ZeroRotator);
	LastHoveredForMoveUnit = nullptr;
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
	else if (bPanButtonDown && (!IsTurn() || !SelectedUnit || (!SelectedUnit->Energy && SelectedUnit->Team == Team)))
		AddActorLocalOffset(FVector(0.f, MouseX, MouseY) * -PanSensitivity * SpringArm->TargetArmLength / 1000);

	
	if (PlayerController != nullptr)
	{
		FHitResult Hit;
		PlayerController->GetHitResultUnderCursor(ECC_GameTraceChannel1, false, Hit);
		HoveredUnit = Cast<AUnit>(Hit.GetActor());
	}

	if (SelectedUnit == nullptr)
		SelectOverlay->GetRootComponent()->SetVisibility(false);
	else
	{
		SelectOverlay->GetRootComponent()->SetVisibility(true);
		SelectOverlay->SetActorLocation(SelectedUnit->GetActorLocation() + FVector(0.f, 0.f, 110.f));
	}
	if (HoveredUnit == nullptr)
		HoverOverlay->GetRootComponent()->SetVisibility(false);
	else
	{
		HoverOverlay->GetRootComponent()->SetVisibility(true);
		HoverOverlay->SetActorLocation(FVector(HoveredUnit->GetActorLocation().X, HoveredUnit->GetActorLocation().Y, 200));
	}
	if (bMoveOverlayIsOn && LastHoveredForMoveUnit != HoveredUnit)
	{
		if (MoveOverlayArray.Num())
			for (auto MoveOverlay : MoveOverlayArray)
				MoveOverlay->Destroy();
		MoveOverlayArray.Empty();
		if (HoveredUnit && MovableTiles.Contains(HoveredUnit->Coordinates))
		{
			UWorld* World = GetWorld();

			int16 yRotation = 90 * static_cast<int>(MovableTiles[HoveredUnit->Coordinates].DirectionToSourceTile);
			MoveOverlayArray.Add(World->SpawnActor<AActor>(MoveArrowBP, 200 * FVector(HoveredUnit->Coordinates % BoardGenerator->BoardWidth, HoveredUnit->Coordinates / BoardGenerator->BoardWidth, 1), FRotator(0, yRotation, 0)));
			int CurrentTileCoordinates = MovableTiles[HoveredUnit->Coordinates].SourceTileCoordinates;
			MoveOverlayArray.Add(World->SpawnActor<AActor>(MoveLineBP, 200 * FVector(CurrentTileCoordinates % BoardGenerator->BoardWidth, CurrentTileCoordinates / BoardGenerator->BoardWidth, 1), FRotator(0, 180 + yRotation, 0)));

			for (int i = 1; i < SelectedUnit->Energy - MovableTiles[HoveredUnit->Coordinates].EnergyLeft; i++)
			{
				yRotation = 90 * static_cast<int>(MovableTiles[CurrentTileCoordinates].DirectionToSourceTile);
				MoveOverlayArray.Add(World->SpawnActor<AActor>(MoveLineBP, 200 * FVector(CurrentTileCoordinates % BoardGenerator->BoardWidth, CurrentTileCoordinates / BoardGenerator->BoardWidth, 1), FRotator(0, yRotation, 0)));
				MoveOverlayArray.Add(World->SpawnActor<AActor>(MoveJointBP, 200 * FVector(CurrentTileCoordinates % BoardGenerator->BoardWidth, CurrentTileCoordinates / BoardGenerator->BoardWidth, 1), FRotator::ZeroRotator));
				CurrentTileCoordinates = MovableTiles[CurrentTileCoordinates].SourceTileCoordinates;
				MoveOverlayArray.Add(World->SpawnActor<AActor>(MoveLineBP, 200 * FVector(CurrentTileCoordinates % BoardGenerator->BoardWidth, CurrentTileCoordinates / BoardGenerator->BoardWidth, 1), FRotator(0, 180 + yRotation, 0)));
			}
			UE_LOG(LogTemp, Display, TEXT("\n-------------------\n"))
		}
		// to check for next tick
		LastHoveredForMoveUnit = HoveredUnit;
	}
	
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
	if (HoveredUnit != nullptr)
	{
		if (HoveredUnit->bIsTargetable)
			SelectedUnit = HoveredUnit;
		else
			SelectedUnit = BoardGenerator->UnitBoard[HoveredUnit->Coordinates];
		bSelectButtonDown = true;
		MovableOverlayOn();
	}
	else
		SelectedUnit = nullptr;
}

void ACameraPawn::MoveUnit()
{
	bSelectButtonDown = false;
	if (SelectedUnit != nullptr && IsTurn() && HoveredUnit != nullptr && MovableTiles.Contains(HoveredUnit->Coordinates) && SelectedUnit->Team == Team)
		BoardGenerator->Server_Move(SelectedUnit->Coordinates, HoveredUnit->Coordinates, MovableTiles[HoveredUnit->Coordinates].EnergyLeft);
	MovableOverlayOff();
}

void ACameraPawn::MovableOverlayOn()
{
	if (SelectedUnit && SelectedUnit->Energy > 0 && BoardGenerator->Turn % 2 == static_cast<int>(Team) && SelectedUnit->Team == Team)
	{
		LastHoveredForMoveUnit = nullptr;
		bMoveOverlayIsOn = true;
		
		TSet<int> TilesForNextPass;
		TSet<int> TilesForCurrentPass;
		TSet<int> TilesPreviouslyQueuedForPassing;

		//TODO: add support for holes in the ground, not with another if statement, but by modifying ground blueprint so that it has no mesh and collision on all sides for the color orange
		
		// Check above tile
		if (// Make sure the tile above is within board bounds
			SelectedUnit->Coordinates / BoardGenerator->BoardWidth > 0
			// Can't move to tile above if already occupied by another unit. Also, no weird maths, because we are never dealing with edge cases, due to the above condition. im so dam smart.
			&& !BoardGenerator->UnitBoard[SelectedUnit->Coordinates - BoardGenerator->BoardWidth]
			// Prevent repeating passing on tiles, since that would result in using up all your energy for any move, even one space moves
			&& !TilesPreviouslyQueuedForPassing.Contains(SelectedUnit->Coordinates - BoardGenerator->BoardWidth)
			// Check if passable upwards, since there are ground tiles that block one direction
			&& !BoardGenerator->GroundBoard[SelectedUnit->Coordinates - BoardGenerator->BoardWidth]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Down)])
		{
			TilesForNextPass.Add(SelectedUnit->Coordinates - BoardGenerator->BoardWidth);
			TilesPreviouslyQueuedForPassing.Add(SelectedUnit->Coordinates - BoardGenerator->BoardWidth);
			MovableTiles.Add(SelectedUnit->Coordinates - BoardGenerator->BoardWidth, { 0, SelectedUnit->Coordinates, EDirection::Down });
		}

		// Same calculations, but right
		if (SelectedUnit->Coordinates % BoardGenerator->BoardWidth < BoardGenerator->BoardWidth - 1 && !BoardGenerator->UnitBoard[SelectedUnit->Coordinates + 1] && !TilesPreviouslyQueuedForPassing.Contains(SelectedUnit->Coordinates + 1) && !BoardGenerator->GroundBoard[SelectedUnit->Coordinates + 1]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Left)])
		{
			TilesForNextPass.Add(SelectedUnit->Coordinates + 1);
			TilesPreviouslyQueuedForPassing.Add(SelectedUnit->Coordinates + 1);
			MovableTiles.Add(SelectedUnit->Coordinates + 1, { 0, SelectedUnit->Coordinates, EDirection::Left });
		}

		// Same calculations, but down
		if (SelectedUnit->Coordinates / BoardGenerator->BoardWidth < BoardGenerator->BoardHeight - 1 && !BoardGenerator->UnitBoard[SelectedUnit->Coordinates + BoardGenerator->BoardWidth] && !TilesPreviouslyQueuedForPassing.Contains(SelectedUnit->Coordinates + BoardGenerator->BoardWidth) && !BoardGenerator->GroundBoard[SelectedUnit->Coordinates + BoardGenerator->BoardWidth]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Up)])
		{
			TilesForNextPass.Add(SelectedUnit->Coordinates + BoardGenerator->BoardWidth);
			TilesPreviouslyQueuedForPassing.Add(SelectedUnit->Coordinates + BoardGenerator->BoardWidth);
			MovableTiles.Add(SelectedUnit->Coordinates + BoardGenerator->BoardWidth, { 0, SelectedUnit->Coordinates, EDirection::Up });
		}

		// Same calculations, but left
		if (SelectedUnit->Coordinates % BoardGenerator->BoardWidth > 0 && !BoardGenerator->UnitBoard[SelectedUnit->Coordinates - 1] && !TilesPreviouslyQueuedForPassing.Contains(SelectedUnit->Coordinates - 1) && !BoardGenerator->GroundBoard[SelectedUnit->Coordinates - 1]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Right)])
		{
			TilesForNextPass.Add(SelectedUnit->Coordinates - 1);
			TilesPreviouslyQueuedForPassing.Add(SelectedUnit->Coordinates - 1);
			MovableTiles.Add(SelectedUnit->Coordinates - 1, { 0, SelectedUnit->Coordinates, EDirection::Right });
		}
		
		for (int16 EnergyLeft = SelectedUnit->Energy - 1; EnergyLeft > 0; EnergyLeft--)
		{
			TilesForCurrentPass = TilesForNextPass;
			TilesForNextPass.Empty();
			for (auto coord : TilesForCurrentPass)
			{
				MovableTiles[coord].EnergyLeft = EnergyLeft;
				if (coord / BoardGenerator->BoardWidth > 0 && !BoardGenerator->UnitBoard[coord - BoardGenerator->BoardWidth] && !TilesPreviouslyQueuedForPassing.Contains(coord - BoardGenerator->BoardWidth) && !BoardGenerator->GroundBoard[coord - BoardGenerator->BoardWidth]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Down)])
				{
					TilesForNextPass.Add(coord - BoardGenerator->BoardWidth);
					TilesPreviouslyQueuedForPassing.Add(coord - BoardGenerator->BoardWidth);
					MovableTiles.Add(coord - BoardGenerator->BoardWidth, { 0, coord, EDirection::Down });
				}

				if (coord % BoardGenerator->BoardWidth < BoardGenerator->BoardWidth - 1 && !BoardGenerator->UnitBoard[coord + 1] && !TilesPreviouslyQueuedForPassing.Contains(coord + 1) && !BoardGenerator->GroundBoard[coord + 1]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Left)])
				{
					TilesForNextPass.Add(coord + 1);
					TilesPreviouslyQueuedForPassing.Add(coord + 1);
					MovableTiles.Add(coord + 1, { 0, coord, EDirection::Left });
				}

				if (coord / BoardGenerator->BoardWidth < BoardGenerator->BoardHeight - 1 && !BoardGenerator->UnitBoard[coord + BoardGenerator->BoardWidth] && !TilesPreviouslyQueuedForPassing.Contains(coord + BoardGenerator->BoardWidth) && !BoardGenerator->GroundBoard[coord + BoardGenerator->BoardWidth]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Up)])
				{
					TilesForNextPass.Add(coord + BoardGenerator->BoardWidth);
					TilesPreviouslyQueuedForPassing.Add(coord + BoardGenerator->BoardWidth);
					MovableTiles.Add(coord + BoardGenerator->BoardWidth, { 0, coord, EDirection::Up });
				}

				if (coord % BoardGenerator->BoardWidth > 0 && !BoardGenerator->UnitBoard[coord - 1] && !TilesPreviouslyQueuedForPassing.Contains(coord - 1) && !BoardGenerator->GroundBoard[coord - 1]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Right)])
				{
					TilesForNextPass.Add(coord - 1);
					TilesPreviouslyQueuedForPassing.Add(coord - 1);
					MovableTiles.Add(coord - 1, { 0, coord, EDirection::Right });
				}
			}
		}
		for (auto& MovableTile : MovableTiles)
			MovableOverlayArray.Add(GetWorld()->SpawnActor<AActor>(MovableOverlayBP, FVector((MovableTile.Key % BoardGenerator->BoardWidth) * 200.f, (MovableTile.Key / BoardGenerator->BoardWidth) * 200.f, 200.f), FRotator::ZeroRotator));
	}
}

void ACameraPawn::MovableOverlayOff()
{
	if (SelectedUnit != nullptr && BoardGenerator->Turn % 2 == static_cast<int>(Team) && SelectedUnit->Team == Team)
	{
		bMoveOverlayIsOn = false;
		for (auto MovableOverlay : MovableOverlayArray)
			MovableOverlay->Destroy();
		MovableOverlayArray.Empty();
		if (MoveOverlayArray.Num())
			for (auto MoveOverlay : MoveOverlayArray)
				MoveOverlay->Destroy();
		MovableOverlayArray.Empty();
		MovableTiles.Empty();
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
	PlayerInputComponent->BindAction("Debug", IE_Pressed, this, &ACameraPawn::Debug);
	PlayerInputComponent->BindAction("Select Unit", IE_Pressed, this, &ACameraPawn::SelectUnit);
	PlayerInputComponent->BindAction("Select Unit", IE_Released, this, &ACameraPawn::MoveUnit);
	PlayerInputComponent->BindAction("Move Unit", IE_Pressed, this, &ACameraPawn::MovableOverlayOn);
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
