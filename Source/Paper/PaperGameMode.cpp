// Copyright (c) 2019–2020 Alden Wu

#include "PaperGameMode.h"
#include "Unit.h"
#include "PaperPlayerController.h"
#include "PaperPlayerState.h"
#include "Castle.h"
#include "Ground.h"
#include "PaperGameState.h"
#include "PaperGameInstance.h"
#include "PaperEnums.h"
#include "Engine/Texture2D.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "stb_image.h"

APaperGameMode::APaperGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APaperGameMode::BeginPlay()
{
	Super::BeginPlay();
	GameState = GetGameState<APaperGameState>();
}

void APaperGameMode::BeginGame()
{
	GameState->Turn = static_cast<uint8>(ETeam::Green);
	GameState->BoardSpawns.AddDefaulted(GameState->TeamCount);


#pragma region Generate Board
	TArray<AUnit*>& UnitBoard = GameState->UnitBoard;
	TArray<AGround*>& GroundBoard = GameState->GroundBoard;
	int& BoardWidth = GameState->BoardWidth;
	int& BoardHeight = GameState->BoardHeight;
	TArray<FColor>& CroppedBoardLayout = GameState->CroppedBoardLayout;

	UnitBoard.Reserve(BoardWidth * BoardHeight);
	GroundBoard.Reserve(BoardWidth * BoardHeight);
	for (int i = 0; i < BoardWidth * BoardHeight; i++)
	{
		UnitBoard.Push(nullptr);
		GroundBoard.Push(nullptr);
	}

	for (int i = 0; i < BoardWidth * BoardHeight; i++)
	{
		FVector SpawnLocation(i % BoardWidth * 200, i / BoardWidth * 200, 0);
		//if not hole, then process, otherwise just skip and never generate
		if (!ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::Hole))
		{
			// spawn ground, because the board should have ground underneath regardless
			GroundBoard[i] = GetWorld()->SpawnActor<AGround>(GroundBP, SpawnLocation, FRotator::ZeroRotator);

			if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::Ground))
				GroundBoard[i]->CollidableDirections = EDirection::Zero;
			// if directional ground
			else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::OneWayU))
				GroundBoard[i]->CollidableDirections = EDirection::Up;
			else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::OneWayR))
				GroundBoard[i]->CollidableDirections = EDirection::Right;
			else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::OneWayD))
				GroundBoard[i]->CollidableDirections = EDirection::Down;
			else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::OneWayL))
				GroundBoard[i]->CollidableDirections = EDirection::Left;
			else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::OneWayUL))
				GroundBoard[i]->CollidableDirections = EDirection::Up | EDirection::Left;
			else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::OneWayUR))
				GroundBoard[i]->CollidableDirections = EDirection::Up | EDirection::Right;
			else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::OneWayDR))
				GroundBoard[i]->CollidableDirections = EDirection::Down | EDirection::Right;
			else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::OneWayDL))
				GroundBoard[i]->CollidableDirections = EDirection::Down | EDirection::Left;

			else
			{
				// if ground not explicitly normal nor directional, then the ground underneath is normal
				GroundBoard[i]->CollidableDirections = EDirection::Zero;
				
				// surface layer now, so spawn 200 units up (size of a block)
				SpawnLocation.Z += 200;

				if		(ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::Wall))
				{
					UnitBoard[i] = GetWorld()->SpawnActor<AUnit>(WallBP, SpawnLocation, FRotator::ZeroRotator);
					UnitBoard[i]->Coordinates = i;
					UnitBoard[i]->Team = ETeam::Neutral;
					UnitBoard[i]->OnRep_Team();
				}
				else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::Mine))
				{
					UnitBoard[i] = GetWorld()->SpawnActor<AUnit>(MineBP, SpawnLocation, FRotator::ZeroRotator);
					UnitBoard[i]->Coordinates = i;
					UnitBoard[i]->Team = ETeam::Neutral;
					UnitBoard[i]->OnRep_Team();
				}
				else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::SpawnGreen))
				{
					AUnit* spawn = GetWorld()->SpawnActor<AUnit>(SpawnBP, SpawnLocation, FRotator::ZeroRotator);
					GameState->BoardSpawns[static_cast<uint8>(ETeam::Green)].Spawns.Push(spawn);
					spawn->Coordinates = i;
					spawn->Team = ETeam::Green;
					spawn->OnRep_Team();
				}
				else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::SpawnRed))
				{
					AUnit* spawn = GetWorld()->SpawnActor<AUnit>(SpawnBP, SpawnLocation, FRotator::ZeroRotator);
					GameState->BoardSpawns[static_cast<uint8>(ETeam::Red)].Spawns.Push(spawn);
					spawn->Coordinates = i;
					spawn->Team = ETeam::Red;
					spawn->OnRep_Team();
				}
				else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::SpawnPurple))
				{
					AUnit* spawn = GetWorld()->SpawnActor<AUnit>(SpawnBP, SpawnLocation, FRotator::ZeroRotator);
					GameState->BoardSpawns[static_cast<uint8>(ETeam::Purple)].Spawns.Push(spawn);
					spawn->Coordinates = i;
					spawn->Team = ETeam::Purple;
					spawn->OnRep_Team();
				}
				else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::SpawnBrown))
				{
					AUnit* spawn = GetWorld()->SpawnActor<AUnit>(SpawnBP, SpawnLocation, FRotator::ZeroRotator);
					GameState->BoardSpawns[static_cast<uint8>(ETeam::Brown)].Spawns.Push(spawn);
					spawn->Coordinates = i;
					spawn->Team = ETeam::Brown;
					spawn->OnRep_Team();
				}
				else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::SpawnWhite))
				{
					AUnit* spawn = GetWorld()->SpawnActor<AUnit>(SpawnBP, SpawnLocation, FRotator::ZeroRotator);
					GameState->BoardSpawns[static_cast<uint8>(ETeam::White)].Spawns.Push(spawn);
					spawn->Coordinates = i;
					spawn->Team = ETeam::White;
					spawn->OnRep_Team();
				}
				else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::SpawnBlack))
				{
					AUnit* spawn = GetWorld()->SpawnActor<AUnit>(SpawnBP, SpawnLocation, FRotator::ZeroRotator);
					GameState->BoardSpawns[static_cast<uint8>(ETeam::Black)].Spawns.Push(spawn);
					spawn->Coordinates = i;
					spawn->Team = ETeam::Black;
					spawn->OnRep_Team();
				}
				else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::CastleGreen))
				{
					UnitBoard[i] = GetWorld()->SpawnActor<ACastle>(CastleBP, SpawnLocation, FRotator::ZeroRotator);
					UnitBoard[i]->Coordinates = i;
					UnitBoard[i]->Team = ETeam::Green;
					UnitBoard[i]->OnRep_Team();
				}
				else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::CastleRed))
				{
					UnitBoard[i] = GetWorld()->SpawnActor<ACastle>(CastleBP, SpawnLocation, FRotator::ZeroRotator);
					UnitBoard[i]->Coordinates = i;
					UnitBoard[i]->Team = ETeam::Red;
					UnitBoard[i]->OnRep_Team();
				}
				else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::CastlePurple))
				{
					UnitBoard[i] = GetWorld()->SpawnActor<ACastle>(CastleBP, SpawnLocation, FRotator::ZeroRotator);
					UnitBoard[i]->Coordinates = i;
					UnitBoard[i]->Team = ETeam::Purple;
					UnitBoard[i]->OnRep_Team();
				}
				else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::CastleBrown))
				{
					UnitBoard[i] = GetWorld()->SpawnActor<ACastle>(CastleBP, SpawnLocation, FRotator::ZeroRotator);
					UnitBoard[i]->Coordinates = i;
					UnitBoard[i]->Team = ETeam::Brown;
					UnitBoard[i]->OnRep_Team();
				}
				else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::CastleWhite))
				{
					UnitBoard[i] = GetWorld()->SpawnActor<ACastle>(CastleBP, SpawnLocation, FRotator::ZeroRotator);
					UnitBoard[i]->Coordinates = i;
					UnitBoard[i]->Team = ETeam::White;
					UnitBoard[i]->OnRep_Team();
				}
				else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::CastleBlack))
				{
					UnitBoard[i] = GetWorld()->SpawnActor<ACastle>(CastleBP, SpawnLocation, FRotator::ZeroRotator);
					UnitBoard[i]->Coordinates = i;
					UnitBoard[i]->Team = ETeam::Black;
					UnitBoard[i]->OnRep_Team();
				}
			}
			GroundBoard[i]->Team = ETeam::Neutral;
			GroundBoard[i]->Coordinates = i;
			GroundBoard[i]->OnRep_Team();
			GroundBoard[i]->BuildArrows();
		}
	}
#pragma endregion

	GameState->bGameStarted = true;
	GameState->Multicast_StartGameForLocalPlayerController();
}

// set player name
APlayerController* APaperGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	APlayerController* PC = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	FString DesignatedName = "";

	if (UGameplayStatics::HasOption(Options, "PlayerName"))
		DesignatedName = UGameplayStatics::ParseOption(Options, "PlayerName");

	if (DesignatedName == "")
		DesignatedName = UGameplayStatics::ParseOption(Options, "Name");

	if (NameCount.Contains(DesignatedName))
	{
		FString Suffix = "_";
		Suffix += FString::FromInt(NameCount[DesignatedName]++);
		DesignatedName += Suffix;
	}
	else
		NameCount.Add(DesignatedName, 1);

	if (APaperPlayerState* PS = PC->GetPlayerState<APaperPlayerState>())
		PS->SetName(DesignatedName);

	if (GameState)
		GameState->Multicast_Message(FText::FromString(DesignatedName + " entered the lobby."));

	return PC;
}

// open team status for that player's team
void APaperGameMode::Logout(AController* Exiting)
{
	if (APaperPlayerController* PC = Cast<APaperPlayerController>(Exiting))
	{
		GameState->Multicast_RemovePlayerForLocalLobbyUI(PC->GetPaperPlayerState()->Name);
		uint8 i = static_cast<uint8>(PC->GetPaperPlayerState()->Team);
		if (i < GameState->TeamCount && GameState->TeamStatuses[i] != EStatus::Dead)
			GameState->TeamStatuses[i] = EStatus::Open;
		GameState->Multicast_Message(FText::FromString(PC->GetPaperPlayerState()->Name + " left the game."));
	}
}

void APaperGameMode::ParseBoardLayoutTexture(const UTexture2D* Texture)
{
	auto OldTeamStatuses = GameState->TeamStatuses;

	ManagedMipMap ManagedBoardLayoutMipMap(&Texture->PlatformData->Mips[0]);
	int BoardLayoutBounds[2][2];
	int& BoardLayoutWidth = ManagedBoardLayoutMipMap->SizeX;
	// reset values in case this wasn't the first time a board was chosen
	GameState->CroppedBoardLayout.Empty();
	GameState->Gold.Empty();
	GameState->CastleHP.Empty();
	GameState->CastleHPMax.Empty();
	GameState->TeamStatuses.Empty();
	GameState->TeamCount = 0;

	// quickly run through texture to determine bounds
	{
		uint8 CurrentBound = 0;
		for (int x = 0; x < BoardLayoutWidth; x++)
			for (int y = 0; y < ManagedBoardLayoutMipMap->SizeY; y++)
				if (ColorsNearlyEqual(ColorCode::Bounds, ManagedBoardLayoutMipMap.GetColorArray()[x + y * BoardLayoutWidth]))
				{
					BoardLayoutBounds[CurrentBound][0] = x;
					BoardLayoutBounds[CurrentBound][1] = y;
					if (CurrentBound)
						goto AfterBoundsDetermined;				// breaking out of nested loops. goto is okay here says stackoverflow
					else
						CurrentBound++;
				}
	}

AfterBoundsDetermined:

	BoardLayoutBounds[0][0]++; BoardLayoutBounds[0][1]++; BoardLayoutBounds[1][0]--; BoardLayoutBounds[1][1]--; // crop unused outline around playable board

	GameState->BoardWidth = BoardLayoutBounds[1][0] - BoardLayoutBounds[0][0] + 1;
	GameState->BoardHeight = BoardLayoutBounds[1][1] - BoardLayoutBounds[0][1] + 1;
	GameState->CroppedBoardLayout.Reserve(GameState->BoardWidth * GameState->BoardHeight);

	// inefficient because processing texture a second time... too bad!
	for (int i = 0; i < GameState->BoardWidth * GameState->BoardHeight; i++)
	{
		// fill in CroppedBoardLayout values
		GameState->CroppedBoardLayout.Add(ManagedBoardLayoutMipMap.GetColorArray()[i % GameState->BoardWidth + BoardLayoutBounds[0][0] + (i / GameState->BoardWidth + BoardLayoutBounds[0][1]) * BoardLayoutWidth]);

		// increment TeamCount accordingly
		if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::SpawnGreen))
		{
			if (GameState->TeamCount <= static_cast<uint8>(ETeam::Green))
				GameState->TeamCount = static_cast<uint8>(ETeam::Green) + 1;			// ETeam values start a 0, and we want the count, so add 1
		}
		else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::SpawnRed))
		{
			if (GameState->TeamCount <= static_cast<uint8>(ETeam::Red))
				GameState->TeamCount = static_cast<uint8>(ETeam::Red) + 1;
		}
		else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::SpawnPurple))
		{
			if (GameState->TeamCount <= static_cast<uint8>(ETeam::Purple))
				GameState->TeamCount = static_cast<uint8>(ETeam::Purple) + 1;
		}
		else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::SpawnBrown))
		{
			if (GameState->TeamCount <= static_cast<uint8>(ETeam::Brown))
				GameState->TeamCount = static_cast<uint8>(ETeam::Brown) + 1;
		}
		else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::SpawnWhite))
		{
			if (GameState->TeamCount <= static_cast<uint8>(ETeam::White))
				GameState->TeamCount = static_cast<uint8>(ETeam::White) + 1;
		}
		else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::SpawnBlack))
		{
			if (GameState->TeamCount <= static_cast<uint8>(ETeam::Black))
				GameState->TeamCount = static_cast<uint8>(ETeam::Black) + 1;
		}
	}

	GameState->OnRep_CroppedBoardLayout();

	GameState->Gold.Reserve(GameState->TeamCount);
	GameState->CastleHP.Reserve(GameState->TeamCount);
	GameState->CastleHPMax.Reserve(GameState->TeamCount);
	GameState->TeamStatuses.Reserve(GameState->TeamCount);
	for (uint8 i = 0; i < GameState->TeamCount; i++)
	{
		GameState->Gold.Add(StartingGold);
		GameState->CastleHP.Add(StartingCastleHP);
		GameState->CastleHPMax.Add(StartingCastleHPMax);
		if (i < OldTeamStatuses.Num())
			GameState->TeamStatuses.Add(OldTeamStatuses[i]);
		else
			GameState->TeamStatuses.Add(EStatus::Open);
	}
	GameState->PassiveIncome = StartingPassiveIncome;
}

bool APaperGameMode::ParseBoardLayoutFile(const FString& Filename)
{
	// backup game state values in case things go wrong
	auto BackupCroppedBoardLayout = GameState->CroppedBoardLayout;
	auto BackupGold = GameState->Gold;
	auto BackupCastleHP = GameState->CastleHP;
	auto BackupCastleHPMax = GameState->CastleHPMax;
	auto BackupTeamStatuses = GameState->TeamStatuses;
	auto BackupTeamCount = GameState->TeamCount;

	// reset values in case this wasn't the first time a board was chosen
	GameState->CroppedBoardLayout.Empty();
	GameState->Gold.Empty();
	GameState->CastleHP.Empty();
	GameState->CastleHPMax.Empty();
	GameState->TeamStatuses.Empty();
	GameState->TeamCount = 0;


	constexpr int PNG_CHANNEL_COUNT = 4;
	int BoardLayoutWidth, BoardLayoutHeight, ChannelCount;
	uint8* BoardImage = stbi_load(TCHAR_TO_UTF8(*Filename), &BoardLayoutWidth, &BoardLayoutHeight, &ChannelCount, PNG_CHANNEL_COUNT);
	// image could not be processed or was not a 32 bit png, so invalid.
	if (!BoardImage || ChannelCount != PNG_CHANNEL_COUNT)
		goto RestoreGameStateValues;


	// quickly run through texture to determine bounds
	int BoardLayoutBounds[2][2];
	{
		uint8 CurrentBound = 0;
		for (int x = 0; x < BoardLayoutWidth; x++)
			for (int y = 0; y < BoardLayoutHeight; y++)
				if (ColorsNearlyEqual(ColorCode::Bounds,
					// Use FColor constructor instead of reinterpret_casting to FColor* because FColor is BGRA not RGBA
					{
						BoardImage[4 * (x + y * BoardLayoutWidth)],					// R
						BoardImage[4 * (x + y * BoardLayoutWidth) + 1],				// G
						BoardImage[4 * (x + y * BoardLayoutWidth) + 2],				// B
						BoardImage[4 * (x + y * BoardLayoutWidth) + 3]				// A
					}))
				{

					BoardLayoutBounds[CurrentBound][0] = x;
					BoardLayoutBounds[CurrentBound][1] = y;
					if (CurrentBound)
						goto AfterBoundsDetermined;				// breaking out of nested loops. goto is okay here says stackoverflow
					else
						CurrentBound++;
				}

		// not enough bounds were detected to goto AfterBoundsDetermined, so invalid.
		goto RestoreGameStateValues;
	}

AfterBoundsDetermined:
	BoardLayoutBounds[0][0]++; BoardLayoutBounds[0][1]++; BoardLayoutBounds[1][0]--; BoardLayoutBounds[1][1]--; // crop unused outline around playable board

	GameState->BoardWidth = BoardLayoutBounds[1][0] - BoardLayoutBounds[0][0] + 1;
	GameState->BoardHeight = BoardLayoutBounds[1][1] - BoardLayoutBounds[0][1] + 1;

	// board width/height aren't positive, so invalid.
	if (GameState->BoardWidth <= 0 || GameState->BoardHeight <= 0)
		goto RestoreGameStateValues;

	GameState->CroppedBoardLayout.Reserve(GameState->BoardWidth * GameState->BoardHeight);

	// inefficient because processing texture a second time... too bad!
	for (int i = 0; i < GameState->BoardWidth * GameState->BoardHeight; i++)
	{
		// fill in CroppedBoardLayout values
		int x = i % GameState->BoardWidth + BoardLayoutBounds[0][0];
		int y = (i / GameState->BoardWidth + BoardLayoutBounds[0][1]);
		GameState->CroppedBoardLayout.Add({
			BoardImage[4 * (x + y * BoardLayoutWidth)],
			BoardImage[4 * (x + y * BoardLayoutWidth) + 1],
			BoardImage[4 * (x + y * BoardLayoutWidth) + 2],
			BoardImage[4 * (x + y * BoardLayoutWidth) + 3]
			});

		// increment TeamCount accordingly
		if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::SpawnGreen))
		{
			if (GameState->TeamCount <= static_cast<uint8>(ETeam::Green))
				GameState->TeamCount = static_cast<uint8>(ETeam::Green) + 1;			// ETeam values start a 0, and we want the count, so add 1
		}
		else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::SpawnRed))
		{
			if (GameState->TeamCount <= static_cast<uint8>(ETeam::Red))
				GameState->TeamCount = static_cast<uint8>(ETeam::Red) + 1;
		}
		else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::SpawnPurple))
		{
			if (GameState->TeamCount <= static_cast<uint8>(ETeam::Purple))
				GameState->TeamCount = static_cast<uint8>(ETeam::Purple) + 1;
		}
		else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::SpawnBrown))
		{
			if (GameState->TeamCount <= static_cast<uint8>(ETeam::Brown))
				GameState->TeamCount = static_cast<uint8>(ETeam::Brown) + 1;
		}
		else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::SpawnWhite))
		{
			if (GameState->TeamCount <= static_cast<uint8>(ETeam::White))
				GameState->TeamCount = static_cast<uint8>(ETeam::White) + 1;
		}
		else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::SpawnBlack))
		{
			if (GameState->TeamCount <= static_cast<uint8>(ETeam::Black))
				GameState->TeamCount = static_cast<uint8>(ETeam::Black) + 1;
		}
	}

	// if there were no board spawns, then invalid.
	if (!GameState->TeamCount)
		goto RestoreGameStateValues;

	GameState->OnRep_CroppedBoardLayout();

	GameState->Gold.Reserve(GameState->TeamCount);
	GameState->CastleHP.Reserve(GameState->TeamCount);
	GameState->CastleHPMax.Reserve(GameState->TeamCount);
	GameState->TeamStatuses.Reserve(GameState->TeamCount);
	for (uint8 i = 0; i < GameState->TeamCount; i++)
	{
		GameState->Gold.Add(StartingGold);
		GameState->CastleHP.Add(StartingCastleHP);
		GameState->CastleHPMax.Add(StartingCastleHPMax);
		if (i < BackupTeamStatuses.Num())
			GameState->TeamStatuses.Add(BackupTeamStatuses[i]);
		else
			GameState->TeamStatuses.Add(EStatus::Open);
	}
	GameState->PassiveIncome = StartingPassiveIncome;

	// everything went well, so valid.
	stbi_image_free(BoardImage);
	return true;

RestoreGameStateValues:
	stbi_image_free(BoardImage);
	GameState->CroppedBoardLayout = BackupCroppedBoardLayout;
	GameState->Gold = BackupGold;
	GameState->CastleHP = BackupCastleHP;
	GameState->CastleHPMax = BackupCastleHPMax;
	GameState->TeamStatuses = BackupTeamStatuses;
	GameState->TeamCount = BackupTeamCount;
	return false;
}

bool APaperGameMode::ColorsNearlyEqual(FColor a, FColor b)
{
	return ((FMath::Abs(a.R - b.R) + FMath::Abs(a.G - b.G) + FMath::Abs(a.B - b.B)) / 255.f < ColorsNearlyEqualThreshold) ? true : false;
}


// APaperGameMode::ManagedMipMap impl

APaperGameMode::ManagedMipMap::ManagedMipMap(FTexture2DMipMap* Source)
{
	MipMap = Source;
	ColorArray = reinterpret_cast<FColor*>(MipMap->BulkData.Lock(LOCK_READ_ONLY));
}

APaperGameMode::ManagedMipMap::~ManagedMipMap()
{
	MipMap->BulkData.Unlock();
}

inline FColor* APaperGameMode::ManagedMipMap::GetColorArray() const
{
	return ColorArray;
}

const FTexture2DMipMap* APaperGameMode::ManagedMipMap::GetMipMap() const
{
	return MipMap;
}

FTexture2DMipMap* APaperGameMode::ManagedMipMap::GetMipMap()
{
	return MipMap;
}

FTexture2DMipMap* APaperGameMode::ManagedMipMap::operator->()
{
	return MipMap;
}

FTexture2DMipMap& APaperGameMode::ManagedMipMap::operator*()
{
	return *MipMap;
}
