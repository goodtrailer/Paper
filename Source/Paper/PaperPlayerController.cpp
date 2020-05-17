// Fill out your copyright notice in the Description page of Project Settings.

#include "PaperPlayerController.h"
#include "Engine/World.h"
#include "GameFramework/PlayerInput.h"

APaperPlayerController::APaperPlayerController()
{
	bAutoManageActiveCameraTarget = false;
	bEnableClickEvents = true;
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Hand;
}

void APaperPlayerController::BeginPlay()
{
	Super::BeginPlay();
	PlayerState = GetPlayerState<APaperPlayerState>();
	GameState = GetWorld()->GetGameState<APaperGameState>();
	CameraPawn = Cast<ACameraPawn>(GetPawn());
	SetViewTargetWithBlend(CameraPawn, 1.f);
	SelectOverlay = GetWorld()->SpawnActor<AActor>(SelectOverlayBP, FVector(0, 0, 200), FRotator::ZeroRotator);
	HoverOverlay = GetWorld()->SpawnActor<AActor>(HoverOverlayBP, FVector(0, 0, 200), FRotator::ZeroRotator);
	LastHoveredForMoveUnit = nullptr;
	UE_LOG(LogTemp, Warning, TEXT("PLAYER INDEX: %d"), PlayerState->PlayerId)
		
}

void APaperPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_GameTraceChannel1, false, Hit);
	HoveredUnit = Cast<AUnit>(Hit.GetActor());

	// Show/hide hover overlay
	if (!HoveredUnit)
		HoverOverlay->GetRootComponent()->SetVisibility(false);
	else
	{
		HoverOverlay->GetRootComponent()->SetVisibility(true);
		HoverOverlay->SetActorLocation(FVector(HoveredUnit->GetActorLocation().X, HoveredUnit->GetActorLocation().Y, 200));
	}

	// (Re)generate movement arrow
	if (bMoveOverlayOn && LastHoveredForMoveUnit != HoveredUnit)
	{
		for (auto MoveOverlay : MoveOverlayArray)
			MoveOverlay->Destroy();
		MoveOverlayArray.Empty();
		if (HoveredUnit && MovableTiles.Contains(HoveredUnit->Coordinates))
		{
			UWorld* World = GetWorld();

			int16 yRotation = 90 * static_cast<int>(MovableTiles[HoveredUnit->Coordinates].DirectionToSourceTile);
			MoveOverlayArray.Add(World->SpawnActor<AActor>(MoveArrowBP, 200 * FVector(HoveredUnit->Coordinates % GameState->GetBoardWidth(), HoveredUnit->Coordinates / GameState->GetBoardWidth(), 1), FRotator(0, yRotation, 0)));
			int CurrentTileCoordinates = MovableTiles[HoveredUnit->Coordinates].SourceTileCoordinates;
			MoveOverlayArray.Add(World->SpawnActor<AActor>(MoveLineBP, 200 * FVector(CurrentTileCoordinates % GameState->GetBoardWidth(), CurrentTileCoordinates / GameState->GetBoardWidth(), 1), FRotator(0, 180 + yRotation, 0)));

			for (int i = 1; i < SelectedUnit->Energy - MovableTiles[HoveredUnit->Coordinates].EnergyLeft; i++)
			{
				yRotation = 90 * static_cast<int>(MovableTiles[CurrentTileCoordinates].DirectionToSourceTile);
				MoveOverlayArray.Add(World->SpawnActor<AActor>(MoveLineBP, 200 * FVector(CurrentTileCoordinates % GameState->GetBoardWidth(), CurrentTileCoordinates / GameState->GetBoardWidth(), 1), FRotator(0, yRotation, 0)));
				MoveOverlayArray.Add(World->SpawnActor<AActor>(MoveJointBP, 200 * FVector(CurrentTileCoordinates % GameState->GetBoardWidth(), CurrentTileCoordinates / GameState->GetBoardWidth(), 1), FRotator::ZeroRotator));
				CurrentTileCoordinates = MovableTiles[CurrentTileCoordinates].SourceTileCoordinates;
				MoveOverlayArray.Add(World->SpawnActor<AActor>(MoveLineBP, 200 * FVector(CurrentTileCoordinates % GameState->GetBoardWidth(), CurrentTileCoordinates / GameState->GetBoardWidth(), 1), FRotator(0, 180 + yRotation, 0)));
			}
		}
		// to check for next tick
		LastHoveredForMoveUnit = HoveredUnit;
	}

}

void APaperPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent)
	InputComponent->BindAction("Rotate Button", IE_Pressed, this, &APaperPlayerController::RotateStart);
	InputComponent->BindAction("Rotate Button", IE_Released, this, &APaperPlayerController::RotateStop);
	InputComponent->BindAction("Pan Button", IE_Pressed, this, &APaperPlayerController::PanStart);
	InputComponent->BindAction("Pan Button", IE_Released, this, &APaperPlayerController::PanStop);
	InputComponent->BindAction("Zoom In", IE_Pressed, this, &APaperPlayerController::ZoomIn);
	InputComponent->BindAction("Zoom Out", IE_Pressed, this, &APaperPlayerController::ZoomOut);
	InputComponent->BindAxis("Mouse X", this, &APaperPlayerController::MouseX);
	InputComponent->BindAxis("Mouse Y", this, &APaperPlayerController::MouseY);
	InputComponent->BindAction("Debug", IE_Pressed, this, &APaperPlayerController::Debug);
	InputComponent->BindAction("Select Unit", IE_Pressed, this, &APaperPlayerController::SelectUnit);
	InputComponent->BindAction("Move Unit", IE_Pressed, this, &APaperPlayerController::MovableOverlayOn);
	InputComponent->BindAction("Move Unit", IE_Released, this, &APaperPlayerController::MoveUnit);
}

/*void APaperPlayerController::InitInputSystem()
{
	Super::InitInputSystem();
	UE_LOG(LogTemp, Display, TEXT("InitInputSystem()"))
		UE_LOG(LogTemp, Display, TEXT("InputComponent->bIsActive: %s"), (InputComponent->bIsActive) ? *FString("true") : *FString("false"))

}*/

void APaperPlayerController::MovableOverlayOff()
{
	bMoveOverlayOn = false;
	if (SelectedUnit != nullptr && PlayerState->IsTurn() && SelectedUnit->Team == PlayerState->Team)
	{
		bMoveOverlayOn = false;
		for (auto MovableOverlay : MovableOverlayArray)
			MovableOverlay->Destroy();
		MovableOverlayArray.Empty();
		MovableTiles.Empty();
		for (auto MoveOverlay : MoveOverlayArray)
			MoveOverlay->Destroy();
		MoveOverlayArray.Empty();
	}
}

#pragma region Input Functions

void APaperPlayerController::RotateStart()
{
	CameraPawn->bIsRotating = true;
}

void APaperPlayerController::RotateStop()
{
	CameraPawn->bIsRotating = false;
}

void APaperPlayerController::PanStart()
{
	if (!(PlayerState->IsTurn() && SelectedUnit && SelectedUnit->Energy && SelectedUnit->Team == PlayerState->Team))
		CameraPawn->bIsPanning = true;
}

void APaperPlayerController::PanStop()
{
	CameraPawn->bIsPanning = false;
}

void APaperPlayerController::ZoomIn()
{
	GLog->Log(TEXT("APaperPlayerController::ZoomIn()"));
	CameraPawn->ZoomIn();
}

void APaperPlayerController::ZoomOut()
{
	GLog->Log(TEXT("APaperPlayerController::ZoomOut()"));
	CameraPawn->ZoomOut();
}

void APaperPlayerController::MouseX(float f)
{
	CameraPawn->MouseDeltaX = f;
}

void APaperPlayerController::MouseY(float f)
{
	CameraPawn->MouseDeltaY = f;
}

void APaperPlayerController::Debug()
{
	GLog->Log(TEXT("Debug()"));
	CameraPawn->SetActorLocation(FVector(2000.f, 2800.f, 300.f));
}

void APaperPlayerController::SelectUnit()
{
	if (HoveredUnit)
		if (HoveredUnit->Type == EType::TypeGround || HoveredUnit->Type == EType::TypeSpawn)
			SelectedUnit = GameState->UnitBoard[HoveredUnit->Coordinates];
		else if (HoveredUnit->bIsTargetable)
			SelectedUnit = HoveredUnit;
		else
			SelectedUnit = nullptr;
	else
		SelectedUnit = nullptr;

	// Show/hide select overlay
	if (!SelectedUnit)
		SelectOverlay->GetRootComponent()->SetVisibility(false);
	else
	{
		SelectOverlay->GetRootComponent()->SetVisibility(true);
		SelectOverlay->SetActorLocation(SelectedUnit->GetActorLocation() + FVector(0.f, 0.f, 110.f));
	}
}

void APaperPlayerController::MovableOverlayOn()
{
	if (SelectedUnit && SelectedUnit->Energy > 0 && PlayerState->IsTurn() && SelectedUnit->Team == PlayerState->Team)
	{
		LastHoveredForMoveUnit = nullptr;
		bMoveOverlayOn = true;

		TSet<int> TilesForNextPass;
		TSet<int> TilesForCurrentPass;
		TSet<int> TilesPreviouslyQueuedForPassing;

		//TODO: add support for holes in the ground, not with another if statement, but by modifying ground blueprint so that it has no mesh and collision on all sides for the color orange

		// Check above tile
		if (// Make sure the tile above is within board bounds
			SelectedUnit->Coordinates / GameState->GetBoardWidth() > 0
			// Can't move to tile above if already occupied by another unit. Also, no weird maths, because we are never dealing with edge cases, due to the above condition. im so dam smart.
			&& !GameState->UnitBoard[SelectedUnit->Coordinates - GameState->GetBoardWidth()]
			// Prevent repeating passing on tiles, since that would result in using up all your energy for any move, even one space moves
			&& !TilesPreviouslyQueuedForPassing.Contains(SelectedUnit->Coordinates - GameState->GetBoardWidth())
			// Check if passable upwards, since there are ground tiles that block one direction
			&& !GameState->GroundBoard[SelectedUnit->Coordinates - GameState->GetBoardWidth()]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Down)])
		{
			TilesForNextPass.Add(SelectedUnit->Coordinates - GameState->GetBoardWidth());
			TilesPreviouslyQueuedForPassing.Add(SelectedUnit->Coordinates - GameState->GetBoardWidth());
			MovableTiles.Add(SelectedUnit->Coordinates - GameState->GetBoardWidth(), { 0, SelectedUnit->Coordinates, EDirection::Down });
		}

		// Same calculations, but right
		if (SelectedUnit->Coordinates % GameState->GetBoardWidth() < GameState->GetBoardWidth() - 1 && !GameState->UnitBoard[SelectedUnit->Coordinates + 1] && !TilesPreviouslyQueuedForPassing.Contains(SelectedUnit->Coordinates + 1) && !GameState->GroundBoard[SelectedUnit->Coordinates + 1]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Left)])
		{
			TilesForNextPass.Add(SelectedUnit->Coordinates + 1);
			TilesPreviouslyQueuedForPassing.Add(SelectedUnit->Coordinates + 1);
			MovableTiles.Add(SelectedUnit->Coordinates + 1, { 0, SelectedUnit->Coordinates, EDirection::Left });
		}

		// Same calculations, but down
		if (SelectedUnit->Coordinates / GameState->GetBoardWidth() < GameState->GetBoardHeight() - 1 && !GameState->UnitBoard[SelectedUnit->Coordinates + GameState->GetBoardWidth()] && !TilesPreviouslyQueuedForPassing.Contains(SelectedUnit->Coordinates + GameState->GetBoardWidth()) && !GameState->GroundBoard[SelectedUnit->Coordinates + GameState->GetBoardWidth()]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Up)])
		{
			TilesForNextPass.Add(SelectedUnit->Coordinates + GameState->GetBoardWidth());
			TilesPreviouslyQueuedForPassing.Add(SelectedUnit->Coordinates + GameState->GetBoardWidth());
			MovableTiles.Add(SelectedUnit->Coordinates + GameState->GetBoardWidth(), { 0, SelectedUnit->Coordinates, EDirection::Up });
		}

		// Same calculations, but left
		if (SelectedUnit->Coordinates % GameState->GetBoardWidth() > 0 && !GameState->UnitBoard[SelectedUnit->Coordinates - 1] && !TilesPreviouslyQueuedForPassing.Contains(SelectedUnit->Coordinates - 1) && !GameState->GroundBoard[SelectedUnit->Coordinates - 1]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Right)])
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
				if (coord / GameState->GetBoardWidth() > 0 && !GameState->UnitBoard[coord - GameState->GetBoardWidth()] && !TilesPreviouslyQueuedForPassing.Contains(coord - GameState->GetBoardWidth()) && !GameState->GroundBoard[coord - GameState->GetBoardWidth()]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Down)])
				{
					TilesForNextPass.Add(coord - GameState->GetBoardWidth());
					TilesPreviouslyQueuedForPassing.Add(coord - GameState->GetBoardWidth());
					MovableTiles.Add(coord - GameState->GetBoardWidth(), { 0, coord, EDirection::Down });
				}

				if (coord % GameState->GetBoardWidth() < GameState->GetBoardWidth() - 1 && !GameState->UnitBoard[coord + 1] && !TilesPreviouslyQueuedForPassing.Contains(coord + 1) && !GameState->GroundBoard[coord + 1]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Left)])
				{
					TilesForNextPass.Add(coord + 1);
					TilesPreviouslyQueuedForPassing.Add(coord + 1);
					MovableTiles.Add(coord + 1, { 0, coord, EDirection::Left });
				}

				if (coord / GameState->GetBoardWidth() < GameState->GetBoardHeight() - 1 && !GameState->UnitBoard[coord + GameState->GetBoardWidth()] && !TilesPreviouslyQueuedForPassing.Contains(coord + GameState->GetBoardWidth()) && !GameState->GroundBoard[coord + GameState->GetBoardWidth()]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Up)])
				{
					TilesForNextPass.Add(coord + GameState->GetBoardWidth());
					TilesPreviouslyQueuedForPassing.Add(coord + GameState->GetBoardWidth());
					MovableTiles.Add(coord + GameState->GetBoardWidth(), { 0, coord, EDirection::Up });
				}

				if (coord % GameState->GetBoardWidth() > 0 && !GameState->UnitBoard[coord - 1] && !TilesPreviouslyQueuedForPassing.Contains(coord - 1) && !GameState->GroundBoard[coord - 1]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Right)])
				{
					TilesForNextPass.Add(coord - 1);
					TilesPreviouslyQueuedForPassing.Add(coord - 1);
					MovableTiles.Add(coord - 1, { 0, coord, EDirection::Right });
				}
			}
		}
		for (auto& MovableTile : MovableTiles)
			MovableOverlayArray.Add(GetWorld()->SpawnActor<AActor>(MovableOverlayBP, FVector((MovableTile.Key % GameState->GetBoardWidth()) * 200.f, (MovableTile.Key / GameState->GetBoardWidth()) * 200.f, 200.f), FRotator::ZeroRotator));
	}
}

void APaperPlayerController::MoveUnit()
{
	if (SelectedUnit && PlayerState->IsTurn() && HoveredUnit && MovableTiles.Contains(HoveredUnit->Coordinates) && SelectedUnit->Team == PlayerState->Team)
	{
		GameState->UnitBoard[HoveredUnit->Coordinates] = GameState->UnitBoard[SelectedUnit->Coordinates];
		GameState->UnitBoard[SelectedUnit->Coordinates] = nullptr;
		GameState->UnitBoard[HoveredUnit->Coordinates]->Coordinates = HoveredUnit->Coordinates;
		GameState->UnitBoard[HoveredUnit->Coordinates]->SetActorLocation(FVector(HoveredUnit->Coordinates % GameState->GetBoardWidth() * 200, HoveredUnit->Coordinates / GameState->GetBoardWidth() * 200, GameState->UnitBoard[HoveredUnit->Coordinates]->GetActorLocation().Z));
		GameState->UnitBoard[HoveredUnit->Coordinates]->Energy = MovableTiles[HoveredUnit->Coordinates].EnergyLeft;
	}
	MovableOverlayOff();
}

#pragma endregion