#include "PaperPlayerController.h"

#include "Unit.h"
#include "PaperGameInstance.h"
#include "PaperPlayerState.h"
#include "CameraPawn.h"
#include "PaperGameState.h"
#include "PaperEnums.h"
#include "Engine/World.h"
#include "GameFramework/PlayerInput.h"

APaperPlayerController::APaperPlayerController()
{
	bAutoManageActiveCameraTarget = false;
	bEnableClickEvents = true;
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void APaperPlayerController::BeginPlay()
{
	Super::BeginPlay();
	GameState = GetWorld()->GetGameState<APaperGameState>();

	CameraPawn = GetPawn<ACameraPawn>();
	SetViewTargetWithBlend(CameraPawn, 1.f);

	UWorld* World = GetWorld();
	SelectOverlay = GetWorld()->SpawnActor<AActor>(SelectOverlayBP, FVector(0, 0, 200), FRotator::ZeroRotator);
	HoverOverlay = GetWorld()->SpawnActor<AActor>(HoverOverlayBP, FVector(0, 0, 200), FRotator::ZeroRotator);
	(AttackOverlay = GetWorld()->SpawnActor<AActor>(AttackOverlayBP, FVector(0, 0, 200), FRotator::ZeroRotator))->GetRootComponent()->SetVisibility(false);
	(MoveOverlay = GetWorld()->SpawnActor<AActor>(MoveOverlayBP, FVector(0, 0, 200), FRotator::ZeroRotator))->GetRootComponent()->SetVisibility(false);
	LastHoveredUnit = nullptr;
}

void APaperPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	FHitResult Hit;
	GetHitResultUnderCursor(ECC_GameTraceChannel1, false, Hit);
	HoveredUnit = Cast<AUnit>(Hit.GetActor());

	
	if (LastHoveredUnit != HoveredUnit)
	{
		// Show/hide/switch hover, attack, and move overlays
		if (HoverOverlay)
		{
			if (!HoveredUnit)
			{
				AttackOverlay->GetRootComponent()->SetVisibility(false);
				HoverOverlay->GetRootComponent()->SetVisibility(false);
				MoveOverlay->GetRootComponent()->SetVisibility(false);
			}
			else if (AttackableTiles.Contains(HoveredUnit->Coordinates))
			{
				AttackOverlay->GetRootComponent()->SetVisibility(true);
				HoverOverlay->GetRootComponent()->SetVisibility(false);
				MoveOverlay->GetRootComponent()->SetVisibility(false);
				AttackOverlay->SetActorLocation(FVector(HoveredUnit->GetActorLocation().X, HoveredUnit->GetActorLocation().Y, 200));
			}
			else if (MovableTiles.Contains(HoveredUnit->Coordinates))
			{
				AttackOverlay->GetRootComponent()->SetVisibility(false);
				HoverOverlay->GetRootComponent()->SetVisibility(false);
				MoveOverlay->GetRootComponent()->SetVisibility(true);
				MoveOverlay->SetActorLocation(FVector(HoveredUnit->GetActorLocation().X, HoveredUnit->GetActorLocation().Y, 200));
			}
			else
			{
				AttackOverlay->GetRootComponent()->SetVisibility(false);
				HoverOverlay->GetRootComponent()->SetVisibility(true);
				MoveOverlay->GetRootComponent()->SetVisibility(false);
				HoverOverlay->SetActorLocation(FVector(HoveredUnit->GetActorLocation().X, HoveredUnit->GetActorLocation().Y, 200));
			}
		}

		// (Re)generate movement arrow
		if (bMovableOverlayOn)
		{
			for (auto& MoveOverlaySegment : MoveOverlayArray)
				MoveOverlaySegment->Destroy();
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
		}
	}


	
	// to check for next tick
	LastHoveredUnit = HoveredUnit;
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
	InputComponent->BindAction("Select Unit", IE_Released, this, &APaperPlayerController::MoveUnit);
	InputComponent->BindAction("Attack", IE_Pressed, this, &APaperPlayerController::ToggleAttackableOverlay);
	InputComponent->BindAction("Move", IE_Pressed, this, &APaperPlayerController::ToggleMovableOverlay);
}

void APaperPlayerController::MovableOverlayOff()
{
	bMovableOverlayOn = false;
	LastHoveredUnit = nullptr;		// update overlays on next tick
	for (auto& MovableOverlay : MovableOverlayArray)
		MovableOverlay->Destroy();
	for (auto& MoveOverlaySegment : MoveOverlayArray)
		MoveOverlaySegment->Destroy();
	MovableOverlayArray.Empty();
	MovableTiles.Empty();
	MoveOverlayArray.Empty();
}

void APaperPlayerController::ToggleMovableOverlay()
{
	if (bMovableOverlayOn)
		MovableOverlayOff();
	else if (!bAttackableOverlayOn)
		MovableOverlayOn();
}

void APaperPlayerController::ToggleAttackableOverlay()
{
	if (bAttackableOverlayOn)
		AttackableOverlayOff();
	else if (!bMovableOverlayOn)
		AttackableOverlayOn();
}


APaperPlayerState* APaperPlayerController::GetPaperPlayerState()
{
	if (UnsafePlayerState)
		return UnsafePlayerState;
	else
		return (UnsafePlayerState = GetPlayerState<APaperPlayerState>());
}

void APaperPlayerController::Server_SpawnUnit_Implementation(TSubclassOf<AUnit> Type)
{
	ETeam Team = GetPaperPlayerState()->Team;
	int BoardWidth = GameState->GetBoardWidth();
	for (int i = 0; i < GameState->BoardSpawns[static_cast<int>(Team)].Spawns.Num(); i++)
		if (GameState->UnitBoard[GameState->GetBoardSpawn(Team, i)->Coordinates] == nullptr)
		{			
			AUnit* SpawnedUnit = GetWorld()->SpawnActor<AUnit>(Type, FVector((GameState->GetBoardSpawn(Team, i)->Coordinates % BoardWidth) * 200, GameState->GetBoardSpawn(Team, i)->Coordinates / BoardWidth * 200, 200), FRotator(0.f));
			GameState->UnitBoard[GameState->GetBoardSpawn(Team, i)->Coordinates] = SpawnedUnit;
			SpawnedUnit->Build(Team);
			SpawnedUnit->Coordinates = GameState->GetBoardSpawn(Team, i)->Coordinates;
			return;
		}
}

bool APaperPlayerController::Server_SpawnUnit_Validate(TSubclassOf<AUnit> Type)
{
	return true;
}

void APaperPlayerController::RotateStart()
{
	if (CameraPawn)
		CameraPawn->bIsRotating = true;
}

void APaperPlayerController::RotateStop()
{
	if (CameraPawn)
		CameraPawn->bIsRotating = false;
}

void APaperPlayerController::PanStart()
{
	if (CameraPawn)
		CameraPawn->bIsPanning = true;
}

void APaperPlayerController::PanStop()
{
	if (CameraPawn)
		CameraPawn->bIsPanning = false;
}

void APaperPlayerController::ZoomIn()
{
	if (CameraPawn)
		CameraPawn->ZoomIn();
}

void APaperPlayerController::ZoomOut()
{
	if (CameraPawn)
		CameraPawn->ZoomOut();
}

void APaperPlayerController::MouseX(float f)
{
	if (CameraPawn)
		CameraPawn->MouseDeltaX = f;
}

void APaperPlayerController::MouseY(float f)
{
	if (CameraPawn)
		CameraPawn->MouseDeltaY = f;
}

void APaperPlayerController::Debug()
{
	if (CameraPawn)
		CameraPawn->SetActorLocation(FVector(2000.f, 2800.f, 300.f));
	if (GetGameInstance<UPaperGameInstance>()->Team == ETeam::TeamNeutral)
		GetGameInstance<UPaperGameInstance>()->Team = ETeam::TeamGreen;
	else
		GetGameInstance<UPaperGameInstance>()->Team = static_cast<ETeam>(static_cast<uint8>(GetGameInstance<UPaperGameInstance>()->Team) + 1);
}

void APaperPlayerController::SelectUnit()
{
	if (bMovableOverlayOn)
		MoveUnit();
	else if (bAttackableOverlayOn)
		AttackUnit();
	else
	{
		// Assign SelectedUnit
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
			GLog->Logf(TEXT("SelectedUnit->HP: %d/%d"), SelectedUnit->GetHP(), SelectedUnit->GetHPMax());
			SelectOverlay->GetRootComponent()->SetVisibility(true);
			SelectOverlay->SetActorLocation(SelectedUnit->GetActorLocation() + FVector(0.f, 0.f, 110.f));
			MovableOverlayOn();
		}
	}
}

void APaperPlayerController::MovableOverlayOn()
{
	if (SelectedUnit && SelectedUnit->Energy > 0 && GetPaperPlayerState()->IsTurn() && SelectedUnit->Team == GetPaperPlayerState()->Team)
	{
		LastHoveredUnit = nullptr;
		bMovableOverlayOn = true;

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
			// Check if ground tile exists upwards, since there could be holes
			&& GameState->GroundBoard[SelectedUnit->Coordinates - GameState->GetBoardWidth()]
			// Check if passable upwards, since there are ground tiles that block one direction
			&& !GameState->GroundBoard[SelectedUnit->Coordinates - GameState->GetBoardWidth()]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Down)])
		{
			TilesForNextPass.Add(SelectedUnit->Coordinates - GameState->GetBoardWidth());
			TilesPreviouslyQueuedForPassing.Add(SelectedUnit->Coordinates - GameState->GetBoardWidth());
			MovableTiles.Add(SelectedUnit->Coordinates - GameState->GetBoardWidth(), { 0, SelectedUnit->Coordinates, EDirection::Down });
		}

		// Same calculations, but right
		if (SelectedUnit->Coordinates % GameState->GetBoardWidth() < GameState->GetBoardWidth() - 1 && !GameState->UnitBoard[SelectedUnit->Coordinates + 1] && !TilesPreviouslyQueuedForPassing.Contains(SelectedUnit->Coordinates + 1) && GameState->GroundBoard[SelectedUnit->Coordinates + 1] && !GameState->GroundBoard[SelectedUnit->Coordinates + 1]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Left)])
		{
			TilesForNextPass.Add(SelectedUnit->Coordinates + 1);
			TilesPreviouslyQueuedForPassing.Add(SelectedUnit->Coordinates + 1);
			MovableTiles.Add(SelectedUnit->Coordinates + 1, { 0, SelectedUnit->Coordinates, EDirection::Left });
		}

		// Same calculations, but down
		if (SelectedUnit->Coordinates / GameState->GetBoardWidth() < GameState->GetBoardHeight() - 1 && !GameState->UnitBoard[SelectedUnit->Coordinates + GameState->GetBoardWidth()] && !TilesPreviouslyQueuedForPassing.Contains(SelectedUnit->Coordinates + GameState->GetBoardWidth()) && GameState->GroundBoard[SelectedUnit->Coordinates + GameState->GetBoardWidth()] && !GameState->GroundBoard[SelectedUnit->Coordinates + GameState->GetBoardWidth()]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Up)])
		{
			TilesForNextPass.Add(SelectedUnit->Coordinates + GameState->GetBoardWidth());
			TilesPreviouslyQueuedForPassing.Add(SelectedUnit->Coordinates + GameState->GetBoardWidth());
			MovableTiles.Add(SelectedUnit->Coordinates + GameState->GetBoardWidth(), { 0, SelectedUnit->Coordinates, EDirection::Up });
		}

		// Same calculations, but left
		if (SelectedUnit->Coordinates % GameState->GetBoardWidth() > 0 && !GameState->UnitBoard[SelectedUnit->Coordinates - 1] && !TilesPreviouslyQueuedForPassing.Contains(SelectedUnit->Coordinates - 1) && GameState->GroundBoard[SelectedUnit->Coordinates - 1] && !GameState->GroundBoard[SelectedUnit->Coordinates - 1]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Right)])
		{
			TilesForNextPass.Add(SelectedUnit->Coordinates - 1);
			TilesPreviouslyQueuedForPassing.Add(SelectedUnit->Coordinates - 1);
			MovableTiles.Add(SelectedUnit->Coordinates - 1, { 0, SelectedUnit->Coordinates, EDirection::Right });
		}

		for (int16 EnergyLeft = SelectedUnit->Energy - 1; EnergyLeft > 0; EnergyLeft--)
		{
			TilesForCurrentPass = TilesForNextPass;
			TilesForNextPass.Empty();
			for (auto& coord : TilesForCurrentPass)
			{
				MovableTiles[coord].EnergyLeft = EnergyLeft;
				if (coord / GameState->GetBoardWidth() > 0 && !GameState->UnitBoard[coord - GameState->GetBoardWidth()] && !TilesPreviouslyQueuedForPassing.Contains(coord - GameState->GetBoardWidth()) && GameState->GroundBoard[coord - GameState->GetBoardWidth()] && !GameState->GroundBoard[coord - GameState->GetBoardWidth()]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Down)])
				{
					TilesForNextPass.Add(coord - GameState->GetBoardWidth());
					TilesPreviouslyQueuedForPassing.Add(coord - GameState->GetBoardWidth());
					MovableTiles.Add(coord - GameState->GetBoardWidth(), { 0, coord, EDirection::Down });
				}

				if (coord % GameState->GetBoardWidth() < GameState->GetBoardWidth() - 1 && !GameState->UnitBoard[coord + 1] && !TilesPreviouslyQueuedForPassing.Contains(coord + 1) && GameState->GroundBoard[coord + 1] && !GameState->GroundBoard[coord + 1]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Left)])
				{
					TilesForNextPass.Add(coord + 1);
					TilesPreviouslyQueuedForPassing.Add(coord + 1);
					MovableTiles.Add(coord + 1, { 0, coord, EDirection::Left });
				}

				if (coord / GameState->GetBoardWidth() < GameState->GetBoardHeight() - 1 && !GameState->UnitBoard[coord + GameState->GetBoardWidth()] && !TilesPreviouslyQueuedForPassing.Contains(coord + GameState->GetBoardWidth()) && GameState->GroundBoard[coord + GameState->GetBoardWidth()] && !GameState->GroundBoard[coord + GameState->GetBoardWidth()]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Up)])
				{
					TilesForNextPass.Add(coord + GameState->GetBoardWidth());
					TilesPreviouslyQueuedForPassing.Add(coord + GameState->GetBoardWidth());
					MovableTiles.Add(coord + GameState->GetBoardWidth(), { 0, coord, EDirection::Up });
				}

				if (coord % GameState->GetBoardWidth() > 0 && !GameState->UnitBoard[coord - 1] && !TilesPreviouslyQueuedForPassing.Contains(coord - 1) && GameState->GroundBoard[coord - 1] && !GameState->GroundBoard[coord - 1]->bIsCollidable.Directions[static_cast<uint8>(EDirection::Right)])
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
	if (SelectedUnit && GetPaperPlayerState()->IsTurn() && HoveredUnit && MovableTiles.Contains(HoveredUnit->Coordinates) && SelectedUnit->Team == GetPaperPlayerState()->Team)
	{
		Server_MoveUnit(SelectedUnit->Coordinates, HoveredUnit->Coordinates, MovableTiles[HoveredUnit->Coordinates].EnergyLeft);
		SelectOverlay->SetActorLocation(FVector(HoveredUnit->Coordinates % GameState->GetBoardWidth() * 200, HoveredUnit->Coordinates / GameState->GetBoardWidth() * 200, 310.f));
	}
	MovableOverlayOff();
}

void APaperPlayerController::AttackUnit()
{
	if (SelectedUnit && GetPaperPlayerState()->IsTurn() && HoveredUnit && AttackableTiles.Contains(HoveredUnit->Coordinates) && SelectedUnit->Team == GetPaperPlayerState()->Team)
	{
		Server_AttackUnit(SelectedUnit, GameState->UnitBoard[HoveredUnit->Coordinates]);
	}
	AttackableOverlayOff();
}

bool APaperPlayerController::Server_AttackUnit_Validate(AUnit* Attacker, AUnit* Victim)
{
	if (Attacker && Victim && Attacker->Energy > 1 && Victim->Team != Attacker->Team)
		return true;
	else
		return false;
}

void APaperPlayerController::Server_AttackUnit_Implementation(AUnit* Attacker, AUnit* Victim)
{
	Attacker->Energy -= 2;
	if (Attacker->Attack < Victim->GetHP())
		Victim->SetHP(Victim->GetHP() - Attacker->Attack);
	else
	{
		Victim->Destroy();
		GameState->UnitBoard[Victim->Coordinates] = nullptr;
	}
		

}

bool APaperPlayerController::Server_MoveUnit_Validate(int Origin, int Destination, uint8 EnergyLeft)
{
	if (Origin < GameState->UnitBoard.Num() && Destination < GameState->UnitBoard.Num() && Destination != Origin)
		return true;
	else
		return false;
}

void APaperPlayerController::Server_MoveUnit_Implementation(int Origin, int Destination, uint8 EnergyLeft)
{
	GameState->UnitBoard[Destination] = GameState->UnitBoard[Origin];
	GameState->UnitBoard[Origin] = nullptr;
	GameState->UnitBoard[Destination]->Coordinates = Destination;
	GameState->UnitBoard[Destination]->OnRep_Coordinates();					// this moves the unit's location on the server too, since onrep calls on clients only (weird)
	GameState->UnitBoard[Destination]->Energy = EnergyLeft;
}

void APaperPlayerController::AttackableOverlayOn()
{
	if (SelectedUnit && SelectedUnit->Energy > 1 && GetPaperPlayerState()->IsTurn() && SelectedUnit->Team == GetPaperPlayerState()->Team)
	{
		bAttackableOverlayOn = true;
		LastHoveredUnit = nullptr;		//update overlays on next tick
		SelectedUnit->DetermineAttackableTiles(ReachableTiles, AttackableTiles);
		for (auto& Coord : ReachableTiles)
			if (GameState->UnitBoard[Coord] == nullptr || AttackableTiles.Contains(Coord))
				AttackableOverlayArray.Add(GetWorld()->SpawnActor<AActor>(AttackableOverlayBP, FVector(Coord % GameState->GetBoardWidth() * 200.f, Coord / GameState->GetBoardWidth() * 200.f, 200.f), FRotator::ZeroRotator));
	}
}

void APaperPlayerController::AttackableOverlayOff()
{
	bAttackableOverlayOn = false;
	LastHoveredUnit = nullptr;		//update overlays on next tick
	for (auto& AttackableOverlay : AttackableOverlayArray)
		AttackableOverlay->Destroy();
	AttackableOverlayArray.Empty();
	AttackableTiles.Empty();
	ReachableTiles.Empty();
}

void APaperPlayerController::Server_EndTurn_Implementation()
{
	GameState->Turn++;
	for (auto Unit : GameState->UnitBoard)
		if (Unit && GameState->Turn % GameState->BoardSpawns.Num() == static_cast<uint8>(Unit->Team))
			Unit->Passive();
}

bool APaperPlayerController::Server_EndTurn_Validate()
{
	return true;
}
