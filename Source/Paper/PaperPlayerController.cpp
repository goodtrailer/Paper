// Copyright (c) 2019–2020 Alden Wu

#include "PaperPlayerController.h"

#include "PaperEnums.h"
#include "CameraPawn.h"
#include "PaperPlayerState.h"
#include "PaperGameState.h"
#include "PaperUserInterface.h"
#include "LobbyUserInterface.h"
#include "ChatUserInterface.h"
#include "Unit.h"
#include "GameFramework/PlayerInput.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "portable-file-dialogs.h"
#include "PaperHUD.h"

APaperPlayerController::APaperPlayerController()
{
	bAutoManageActiveCameraTarget = false;
	bEnableClickEvents = true;
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	bInGame = false;
}

void APaperPlayerController::BeginPlay()
{
	Super::BeginPlay();
	GameState = GetWorld()->GetGameState<APaperGameState>();
	HUD = GetHUD<APaperHUD>();

	CameraPawn = GetPawn<ACameraPawn>();
	SetViewTargetWithBlend(CameraPawn, 1.f);

	if (IsLocalPlayerController())
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);

		(LobbyInterface = CreateWidget<ULobbyUserInterface>(this, LobbyInterfaceBP))->AddToViewport();
		(ChatInterface = CreateWidget<UChatUserInterface>(this, ChatInterfaceBP))->AddToViewport();
		UserInterface = CreateWidget<UPaperUserInterface>(this, UserInterfaceBP);

		(SelectOverlay = GetWorld()->SpawnActor<AActor>(SelectOverlayBP, FVector(0, 0, 200), FRotator::ZeroRotator))->GetRootComponent()->SetVisibility(false);
		(HoverOverlay = GetWorld()->SpawnActor<AActor>(HoverOverlayBP, FVector(0, 0, 200), FRotator::ZeroRotator))->GetRootComponent()->SetVisibility(false);
		(AttackOverlay = GetWorld()->SpawnActor<AActor>(AttackOverlayBP, FVector(0, 0, 200), FRotator::ZeroRotator))->GetRootComponent()->SetVisibility(false);
		(MoveOverlay = GetWorld()->SpawnActor<AActor>(MoveOverlayBP, FVector(0, 0, 200), FRotator::ZeroRotator))->GetRootComponent()->SetVisibility(false);
		(SpawnableOverlay = GetWorld()->SpawnActor<AActor>(SpawnableOverlayBP, FVector(0, 0, 200), FRotator::ZeroRotator))->GetRootComponent()->SetVisibility(false);
		(NonspawnableOverlay = GetWorld()->SpawnActor<AActor>(NonspawnableOverlayBP, FVector(0, 0, 200), FRotator::ZeroRotator))->GetRootComponent()->SetVisibility(false);
	}

	LastHoveredUnit = nullptr;
	bInGame = false;
}

void APaperPlayerController::StartGame()
{
	if (!GameState || !GetPlayerState<APlayerState>())
		return;

	Server_SetInGame(true);

	CameraPawn->ResetPosition();

	// switch ui and update all the widgets
	LobbyInterface->RemoveFromViewport();
	UserInterface->AddToViewport();
	UserInterface->UpdateTurn(GetPaperPlayerState()->IsTurn());
	UserInterface->UpdateTeam(GetPaperPlayerState()->Team);

	// Log to other players that player has joined the game as <team>. thanks legacy wiki :)
	FString TeamString;
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ETeam"), true);
	if (!EnumPtr)
		TeamString = "INVALID";
	else
		TeamString = EnumPtr->GetNameByValue(static_cast<int64>(GetPaperPlayerState()->Team)).ToString();
	TeamString.RemoveFromStart("ETeam::");
	TeamString.ToUpperInline();
	GameState->Multicast_Message(FText::FromString(GetPaperPlayerState()->Name + " joined the game as " + TeamString + "."));
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
	InputComponent->BindAction("Reset Camera", IE_Pressed, this, &APaperPlayerController::ResetCameraPosition);
	InputComponent->BindAction("Select Unit", IE_Pressed, this, &APaperPlayerController::SelectUnit);
	InputComponent->BindAction("Attack", IE_Pressed, this, &APaperPlayerController::ToggleAttackableOverlay);
	InputComponent->BindAction("Move", IE_Pressed, this, &APaperPlayerController::ToggleMovableOverlay);
	InputComponent->BindAction("Chat", IE_Pressed, this, &APaperPlayerController::FocusChatbox);
	InputComponent->BindAction("End Turn", IE_Pressed, this, &APaperPlayerController::Server_EndTurn);
	InputComponent->BindAction("Menu", IE_Pressed, this, &APaperPlayerController::ToggleMenu);
	InputComponent->BindAction("Resize HP Bar Show Radius", IE_Pressed, this, &APaperPlayerController::ResizeHPBarShowRadiusStart);
	InputComponent->BindAction("Resize HP Bar Show Radius", IE_Released, this, &APaperPlayerController::ResizeHPBarShowRadiusStop);
	InputComponent->BindAction("Scoreboard", IE_Pressed, this, &APaperPlayerController::ShowScoreboard);
	InputComponent->BindAction("Scoreboard", IE_Released, this, &APaperPlayerController::HideScoreboard);
}



void APaperPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (!bInGame)
		return;

	FHitResult Hit;
	GetHitResultUnderCursor(ECC_GameTraceChannel1, false, Hit);
	HoveredUnit = Cast<AUnit>(Hit.GetActor());


	if (LastHoveredUnit != HoveredUnit)
	{
		// Show/hide/switch hover, attack, and move overlays
		if (!HoveredUnit)
		{
			AttackOverlay->GetRootComponent()->SetVisibility(false);
			HoverOverlay->GetRootComponent()->SetVisibility(false);
			MoveOverlay->GetRootComponent()->SetVisibility(false);
			SpawnableOverlay->GetRootComponent()->SetVisibility(false);
			NonspawnableOverlay->GetRootComponent()->SetVisibility(false);
		}
		else if (AttackableTiles.Contains(HoveredUnit->Coordinates))
		{
			AttackOverlay->GetRootComponent()->SetVisibility(true);
			HoverOverlay->GetRootComponent()->SetVisibility(false);
			MoveOverlay->GetRootComponent()->SetVisibility(false);
			SpawnableOverlay->GetRootComponent()->SetVisibility(false);
			NonspawnableOverlay->GetRootComponent()->SetVisibility(false);

			AttackOverlay->SetActorLocation(FVector(HoveredUnit->GetActorLocation().X, HoveredUnit->GetActorLocation().Y, 200));
		}
		else if (MovableTiles.Contains(HoveredUnit->Coordinates))
		{
			AttackOverlay->GetRootComponent()->SetVisibility(false);
			HoverOverlay->GetRootComponent()->SetVisibility(false);
			MoveOverlay->GetRootComponent()->SetVisibility(true);
			SpawnableOverlay->GetRootComponent()->SetVisibility(false);
			NonspawnableOverlay->GetRootComponent()->SetVisibility(false);

			MoveOverlay->SetActorLocation(FVector(HoveredUnit->GetActorLocation().X, HoveredUnit->GetActorLocation().Y, 200));
		}
		else if (bSpawnableOverlayOn)
		{
			AttackOverlay->GetRootComponent()->SetVisibility(false);
			HoverOverlay->GetRootComponent()->SetVisibility(false);
			MoveOverlay->GetRootComponent()->SetVisibility(false);
			if (SpawnableTiles.Contains(HoveredUnit->Coordinates))
			{
				SpawnableOverlay->GetRootComponent()->SetVisibility(true);
				NonspawnableOverlay->GetRootComponent()->SetVisibility(false);

				SpawnableOverlay->SetActorLocation(FVector(HoveredUnit->GetActorLocation().X, HoveredUnit->GetActorLocation().Y, 200));
			}
			else
			{
				SpawnableOverlay->GetRootComponent()->SetVisibility(false);
				NonspawnableOverlay->GetRootComponent()->SetVisibility(true);

				NonspawnableOverlay->SetActorLocation(FVector(HoveredUnit->GetActorLocation().X, HoveredUnit->GetActorLocation().Y, 200));
			}
		}
		else
		{
			AttackOverlay->GetRootComponent()->SetVisibility(false);
			HoverOverlay->GetRootComponent()->SetVisibility(true);
			MoveOverlay->GetRootComponent()->SetVisibility(false);
			SpawnableOverlay->GetRootComponent()->SetVisibility(false);
			NonspawnableOverlay->GetRootComponent()->SetVisibility(false);

			HoverOverlay->SetActorLocation(FVector(HoveredUnit->GetActorLocation().X, HoveredUnit->GetActorLocation().Y, 200));
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

				int16 yRotation = EDirectionToDegrees(MovableTiles[HoveredUnit->Coordinates].DirectionToSourceTile);
				MoveOverlayArray.Add(World->SpawnActor<AActor>(MoveArrowBP, 200 * FVector(HoveredUnit->Coordinates % GameState->GetBoardWidth(), HoveredUnit->Coordinates / GameState->GetBoardWidth(), 1), FRotator(0, yRotation, 0)));
				int CurrentTileCoordinates = MovableTiles[HoveredUnit->Coordinates].SourceTileCoordinates;
				MoveOverlayArray.Add(World->SpawnActor<AActor>(MoveLineBP, 200 * FVector(CurrentTileCoordinates % GameState->GetBoardWidth(), CurrentTileCoordinates / GameState->GetBoardWidth(), 1), FRotator(0, 180 + yRotation, 0)));

				for (int i = 1; i < SelectedUnit->Energy - MovableTiles[HoveredUnit->Coordinates].EnergyLeft; i++)
				{
					yRotation = EDirectionToDegrees(MovableTiles[CurrentTileCoordinates].DirectionToSourceTile);
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



APaperPlayerState* APaperPlayerController::GetPaperPlayerState()
{
	if (UnsafePlayerState)
		return UnsafePlayerState;
	else
		return (UnsafePlayerState = GetPlayerState<APaperPlayerState>());
}



void APaperPlayerController::RotateStart()
{
	if (bInGame)
		CameraPawn->bIsRotating = true;
}

void APaperPlayerController::RotateStop()
{
	if (bInGame)
		CameraPawn->bIsRotating = false;
}

void APaperPlayerController::PanStart()
{
	if (bInGame)
		CameraPawn->bIsPanning = true;
}

void APaperPlayerController::PanStop()
{
	if (bInGame)
		CameraPawn->bIsPanning = false;
}

void APaperPlayerController::ZoomIn()
{
	if (HUD->bShowRadiusThresholdCircle)
		HUD->CursorRadiusThreshold += max(HUD->CursorRadiusThreshold * .15f, 1);		// radius can hit 0 due to rounding, and simply doing *= 1.1f means it won't get larger
	else if (bInGame)
		CameraPawn->ZoomIn();
}

void APaperPlayerController::ZoomOut()
{
	if (HUD->bShowRadiusThresholdCircle)
		HUD->CursorRadiusThreshold *= .85f;
	else if (bInGame)
		CameraPawn->ZoomOut();
}

void APaperPlayerController::MouseX(float f)
{
	if (bInGame)
		CameraPawn->MouseDeltaX = f;
}

void APaperPlayerController::MouseY(float f)
{
	if (bInGame)
		CameraPawn->MouseDeltaY = f;
}

void APaperPlayerController::ResetCameraPosition()
{
	if (bInGame)
		CameraPawn->ResetPosition();
}

void APaperPlayerController::Debug()
{
	ResetCameraPosition();
}

void APaperPlayerController::FocusChatbox()
{
	ChatInterface->FocusChatbox();
}



void APaperPlayerController::SelectUnit()
{
	if (!bInGame)
		return;

	// Move unit
	if (bMovableOverlayOn)
	{
		if (SelectedUnit && HoveredUnit)
			Server_MoveUnit(SelectedUnit->Coordinates, HoveredUnit->Coordinates);
		MovableOverlayOff();
	}
	// Attack unit
	else if (bAttackableOverlayOn)
	{
		if (HoveredUnit)
			Server_Attack(SelectedUnit, GameState->UnitBoard[HoveredUnit->Coordinates]);
		AttackableOverlayOff();
	}
	// Spawn unit
	else if (bSpawnableOverlayOn)
	{
		if (HoveredUnit)
			Server_SpawnUnit(UnitToSpawnBP, HoveredUnit->Coordinates);
		SpawnableOverlayOff();
	}
	// Select unit
	else
	{
		// Assign SelectedUnit
		if (HoveredUnit)
			if (HoveredUnit->Type == EType::Ground || HoveredUnit->Type == EType::Spawn)
				SelectedUnit = GameState->UnitBoard[HoveredUnit->Coordinates];
			else if (HoveredUnit->bIsTargetable)
				SelectedUnit = HoveredUnit;
			else
				SelectedUnit = nullptr;
		else
			SelectedUnit = nullptr;
	}
}

void APaperPlayerController::ToggleMenu()
{
	UserInterface->ToggleMenu();
}

void APaperPlayerController::ShowScoreboard()
{
	if (bInGame)
		(ScoreboardInterface = CreateWidget<UUserWidget>(this, ScoreboardInterfaceBP))->AddToViewport();
}

void APaperPlayerController::HideScoreboard()
{
	if (ScoreboardInterface && bInGame)
		ScoreboardInterface->RemoveFromViewport();
}

void APaperPlayerController::ResizeHPBarShowRadiusStart()
{
	GLog->Log(L"Start");
	HUD->bShowRadiusThresholdCircle = true;
}

void APaperPlayerController::ResizeHPBarShowRadiusStop()
{
	HUD->bShowRadiusThresholdCircle = false;
}


void APaperPlayerController::CheckDeadUnit(AUnit* Unit)
{
	if (SelectedUnit == Unit)
			SelectedUnit = nullptr;
}



void APaperPlayerController::ToggleSpawnableOverlay(TSubclassOf<AUnit> Type)
{
	if (bSpawnableOverlayOn && Type == UnitToSpawnBP)
		SpawnableOverlayOff();
	else
		SpawnableOverlayOn(Type);
}

void APaperPlayerController::SpawnableOverlayOn(TSubclassOf<AUnit> Type)
{
	const int TeamIndex = static_cast<int>(GetPaperPlayerState()->Team);
	for (auto& Spawn : GameState->BoardSpawns[TeamIndex].Spawns)
		SpawnableTiles.Add(Spawn->Coordinates);
	UnitToSpawnBP = Type;
	bSpawnableOverlayOn = true;
	LastHoveredUnit = nullptr;			// update overlays in playertick
}

void APaperPlayerController::SpawnableOverlayOff()
{
	SpawnableTiles.Empty();
	bSpawnableOverlayOn = false;
	LastHoveredUnit = nullptr;			// update overlays on next tick
}

bool APaperPlayerController::Server_SpawnUnit_Validate(TSubclassOf<AUnit> Type, int Coordinates)
{
	return true;
}

void APaperPlayerController::Server_SpawnUnit_Implementation(TSubclassOf<AUnit> Type, int Coordinates)
{
	ETeam& Team = GetPaperPlayerState()->Team;
	int Cost = Type.GetDefaultObject()->Cost;
	if (GetPaperPlayerState()->IsTurn()
		&& GameState->GetGold(Team) >= Cost)
		// recheck if coordinates are actually a spawn, because SpawnableTiles is calculated/stored on client, but this is a server rpc
		for (auto& Spawn : GameState->BoardSpawns[static_cast<int>(GetPaperPlayerState()->Team)].Spawns)
			if (Spawn->Coordinates == Coordinates)
			{
				if (GameState->UnitBoard[Coordinates] == nullptr)
				{
					int BoardWidth = GameState->GetBoardWidth();
					#if WITH_EDITOR
						FActorSpawnParameters SpawnParams;
						SpawnParams.bHideFromSceneOutliner = false;
						AUnit* SpawnedUnit = GetWorld()->SpawnActor<AUnit>(Type, FVector(Coordinates % BoardWidth * 200, Coordinates / BoardWidth * 200, 200), FRotator(0.f), SpawnParams);
					#else
						AUnit* SpawnedUnit = GetWorld()->SpawnActor<AUnit>(Type, FVector(Coordinates % BoardWidth * 200, Coordinates / BoardWidth * 200, 200), FRotator(0.f));
					#endif

					SpawnedUnit->SetOwner(this);
					GameState->UnitBoard[Coordinates] = SpawnedUnit;
					GameState->ChangeGold(Team, -Cost);
					SpawnedUnit->Team = Team;
					SpawnedUnit->OnRep_Team();
					SpawnedUnit->Coordinates = Coordinates;
					break;
				}
				break;
			}
}



void APaperPlayerController::ToggleMovableOverlay()
{
	if (bMovableOverlayOn)
		MovableOverlayOff();
	else if (!bAttackableOverlayOn)
		MovableOverlayOn();
}

void APaperPlayerController::MovableOverlayOn()
{
	if (SelectedUnit && SelectedUnit->Energy > 0 && GetPaperPlayerState()->IsTurn() && SelectedUnit->Team == GetPaperPlayerState()->Team)
	{
		LastHoveredUnit = nullptr;
		bMovableOverlayOn = true;

		// determine movable tiles
		SelectedUnit->DetermineMovableTiles(MovableTiles);

		for (auto& MovableTile : MovableTiles)				// instantiate movable overlays for each determined movable tile
			MovableOverlayArray.Add(GetWorld()->SpawnActor<AActor>(MovableOverlayBP, FVector((MovableTile.Key % GameState->GetBoardWidth()) * 200.f, (MovableTile.Key / GameState->GetBoardWidth()) * 200.f, 200.f), FRotator::ZeroRotator));
	}
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

bool APaperPlayerController::Server_MoveUnit_Validate(int Origin, int Destination)
{
	if (Origin < GameState->UnitBoard.Num() && Destination < GameState->UnitBoard.Num())			// kick because player is SUS AS HELL if they manage to break this
		return true;
	else
		return false;
}

void APaperPlayerController::Server_MoveUnit_Implementation(int Origin, int Destination)
{

	if (GameState->UnitBoard[Origin]			// the unit exists
		&& GameState->UnitBoard[Origin]->Team == GetPaperPlayerState()->Team			// it's your own unit
		&& GetPaperPlayerState()->IsTurn()
		&& Destination != Origin)					// it's your turn
	{
		GameState->UnitBoard[Origin]->DetermineMovableTiles(MovableTiles);
		if (MovableTiles.Contains(Destination))				// the destination is valid (i.e. within reach) for the unit
		{
			GameState->UnitBoard[Destination] = GameState->UnitBoard[Origin];
			GameState->UnitBoard[Origin] = nullptr;
			GameState->UnitBoard[Destination]->Coordinates = Destination;
			GameState->UnitBoard[Destination]->Energy = MovableTiles[Destination].EnergyLeft;
			GameState->UnitBoard[Destination]->SetActorLocation(FVector(Destination % GameState->GetBoardWidth() * 200, Destination / GameState->GetBoardWidth() * 200, GameState->UnitBoard[Destination]->GetActorLocation().Z));
		}
	}
}



void APaperPlayerController::ToggleAttackableOverlay()
{
	if (bAttackableOverlayOn)
		AttackableOverlayOff();
	else if (!bMovableOverlayOn)
		AttackableOverlayOn();
}

void APaperPlayerController::AttackableOverlayOn()
{
	if (SelectedUnit && SelectedUnit->Energy > 1 && GetPaperPlayerState()->IsTurn() && SelectedUnit->Team == GetPaperPlayerState()->Team)
	{
		bAttackableOverlayOn = true;
		LastHoveredUnit = nullptr;		//update overlays on next tick

		SelectedUnit->DetermineAttackableTiles(ReachableTiles, AttackableTiles);					// COSMETIC CALCULATIONS
		for (auto& Coord : ReachableTiles)
			if (GameState->UnitBoard[Coord] == nullptr || AttackableTiles.Contains(Coord))
				AttackableOverlayArray.Add(GetWorld()->SpawnActor<AActor>(AttackableOverlayBP, FVector(Coord % GameState->GetBoardWidth() * 200.f, Coord / GameState->GetBoardWidth() * 200.f, 200.f), FRotator::ZeroRotator));
	}
}

void APaperPlayerController::AttackableOverlayOff()													// COSMETIC
{
	bAttackableOverlayOn = false;
	LastHoveredUnit = nullptr;		//update overlays on next tick
	for (auto& AttackableOverlay : AttackableOverlayArray)
		AttackableOverlay->Destroy();
	AttackableOverlayArray.Empty();
	AttackableTiles.Empty();
	ReachableTiles.Empty();
}

bool APaperPlayerController::Server_Attack_Validate(AUnit* Attacker, AUnit* Victim)
{
	return true;
}

void APaperPlayerController::Server_Attack_Implementation(AUnit* Attacker, AUnit* Victim)
{
	if (Attacker
		&& Victim
		&& Attacker->Energy > 1
		&& GetPaperPlayerState()->IsTurn()
		&& Attacker->Team == GetPaperPlayerState()->Team)
	{
		Attacker->DetermineAttackableTiles(ReachableTiles, AttackableTiles);		// doing this last because it's taxing, use as final barrier.
		if (AttackableTiles.Contains(Victim->Coordinates))
			Attacker->Attack(Victim);
	}
}



bool APaperPlayerController::Server_EndTurn_Validate()
{
	return true;
}

void APaperPlayerController::Server_EndTurn_Implementation()
{
	if (GetPaperPlayerState()->IsTurn())															// GAMEPLAY CALCULATIONS
		GameState->EndTurn();
}



bool APaperPlayerController::Server_ChangeTeam_Validate(ETeam Team)
{
	if ((static_cast<uint8>(Team) < GameState->TeamCount || Team == ETeam::Neutral) && !bInGame)				// this should never happen, so kick because you're sus
		return true;
	else
		return false;
}

void APaperPlayerController::Server_ChangeTeam_Implementation(ETeam Team)			// checks are to prevent clients from setting ingame to false and hijacking someone else's team.
{
	if (Team == ETeam::Neutral || GameState->TeamStatuses[static_cast<uint8>(Team)] == EStatus::Open)			// if team is open
	{
		// mark current team as open if alive. note: current team can't be open, because you're currently occupying it, and if it's already dead, leave it as dead.
		if (GetPaperPlayerState()->Team != ETeam::Neutral && GameState->TeamStatuses[static_cast<uint8>(GetPaperPlayerState()->Team)] == EStatus::Alive)
			GameState->TeamStatuses[static_cast<uint8>(GetPaperPlayerState()->Team)] = EStatus::Open;

		// then switch to new team and mark it as taken
		GetPaperPlayerState()->SetTeam(Team);
		if (Team != ETeam::Neutral)
			GameState->TeamStatuses[static_cast<uint8>(Team)] = EStatus::Alive;
	}
}



bool APaperPlayerController::Server_SetInGame_Validate(bool b)
{
	return true;
}

void APaperPlayerController::Server_SetInGame_Implementation(bool b)
{
	bInGame = b;
}


bool APaperPlayerController::Server_SendMessage_Validate(const FText& Message)
{
	return true;
}

void APaperPlayerController::Server_SendMessage_Implementation(const FText& Message)
{
	// no checks because it's just a message, no real harm possible
	constexpr int MAX_SIGNATURE_LENGTH = 16;
	FText Signature;

	if (GetPaperPlayerState()->Name.Len() > MAX_SIGNATURE_LENGTH)
		Signature = FText::FromString(GetPaperPlayerState()->Name.Left(MAX_SIGNATURE_LENGTH) + "...");
	else
		Signature = FText::FromString(GetPaperPlayerState()->Name);

	FText SignedMessage = FText::Format<FText, FText>(FText::FromString("{0}: {1}"), Signature, Message);
	GameState->Multicast_Message(SignedMessage);
}

const FString APaperPlayerController::OpenFile(const FString& Title, const FString& DefaultPath, const TArray<FString>& FileTypes)
{

	if (!FPaths::DirectoryExists(DefaultPath))
		IFileManager::Get().MakeDirectory(*DefaultPath);

	std::vector<std::string> FileTypesVec;
	FileTypesVec.reserve(FileTypes.Num());
	for (auto& String : FileTypes)
		FileTypesVec.emplace_back(TCHAR_TO_ANSI(*String));

	std::vector<std::string> selection = pfd::open_file::open_file(TCHAR_TO_ANSI(*Title), TCHAR_TO_ANSI(*DefaultPath), FileTypesVec).result();
	return FString(selection.empty() ? L"" : ANSI_TO_TCHAR(selection[0].c_str()));
}


void APaperPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APaperPlayerController, bInGame);
}
