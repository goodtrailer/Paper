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

#define SPAWN_UNIT(CLASS, TEAM, COORDS) { \
	FVector SpawnLoc(COORDS % GameState->BoardWidth * 200, COORDS / GameState->BoardWidth * 200, 200); \
	GameState->UnitBoard[COORDS] = GetWorld()->SpawnActor<AUnit>(CLASS, SpawnLoc, FRotator::ZeroRotator); \
	GameState->UnitBoard[COORDS]->Coordinates = COORDS; \
	GameState->UnitBoard[COORDS]->Team = TEAM; \
	GameState->UnitBoard[COORDS]->OnRep_Team(); \
	GameState->UnitBoard[COORDS]->BuildMisc(); \
}

#define SPAWN_SPAWN(TEAM, COORDS) { \
	FVector SpawnLoc(COORDS % GameState->BoardWidth * 200, COORDS / GameState->BoardWidth * 200, 200); \
	AUnit* spawn = GetWorld()->SpawnActor<AUnit>(SpawnBP, SpawnLoc, FRotator::ZeroRotator); \
	GameState->BoardSpawns[static_cast<uint8>(TEAM)].Spawns.Push(spawn); \
	spawn->Coordinates = COORDS; \
	spawn->Team = TEAM; \
	spawn->OnRep_Team(); \
	spawn->BuildMisc(); \
}

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

	// acts as a Reserve() because AddZeroed(i) calls Reserve(i), except it also fills with nullptr
	GameState->UnitBoard.AddZeroed(GameState->BoardWidth * GameState->BoardHeight);
	GameState->GroundBoard.AddZeroed(GameState->BoardWidth * GameState->BoardHeight);

	for (int i = 0; i < GameState->BoardWidth * GameState->BoardHeight; i++)
	{
		//if not hole, then process, otherwise just skip and never generate
		if (!ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::Hole))
		{
			// spawn ground, because the board should have ground underneath regardless
			GameState->GroundBoard[i] = GetWorld()->SpawnActor<AGround>(GroundBP, FVector(i % GameState->BoardWidth * 200, i / GameState->BoardWidth * 200, 0.f), FRotator::ZeroRotator);

			if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::Ground))
				GameState->GroundBoard[i]->CollidableDirections = EDirection::Zero;
			// if directional ground
			else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::OneWayU))
				GameState->GroundBoard[i]->CollidableDirections = EDirection::Up;
			else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::OneWayR))
				GameState->GroundBoard[i]->CollidableDirections = EDirection::Right;
			else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::OneWayD))
				GameState->GroundBoard[i]->CollidableDirections = EDirection::Down;
			else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::OneWayL))
				GameState->GroundBoard[i]->CollidableDirections = EDirection::Left;
			else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::OneWayUL))
				GameState->GroundBoard[i]->CollidableDirections = EDirection::Up | EDirection::Left;
			else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::OneWayUR))
				GameState->GroundBoard[i]->CollidableDirections = EDirection::Up | EDirection::Right;
			else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::OneWayDR))
				GameState->GroundBoard[i]->CollidableDirections = EDirection::Down | EDirection::Right;
			else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::OneWayDL))
				GameState->GroundBoard[i]->CollidableDirections = EDirection::Down | EDirection::Left;

			else
			{
				// if ground not explicitly normal nor directional, then the ground underneath is normal
				GameState->GroundBoard[i]->CollidableDirections = EDirection::Zero;

				if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::Wall))
					SPAWN_UNIT(WallBP, ETeam::Neutral, i)
				else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::Mine))
					SPAWN_UNIT(MineBP, ETeam::Neutral, i)
				else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::SpawnGreen))
					SPAWN_SPAWN(ETeam::Green, i)
				else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::SpawnRed))
					SPAWN_SPAWN(ETeam::Red, i)
				else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::SpawnPurple))
					SPAWN_SPAWN(ETeam::Purple, i)
				else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::SpawnBrown))
					SPAWN_SPAWN(ETeam::Brown, i)
				else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::SpawnWhite))
					SPAWN_SPAWN(ETeam::White, i)
				else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::SpawnBlack))
					SPAWN_SPAWN(ETeam::Black, i)
				else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::CastleGreen))
					SPAWN_UNIT(CastleBP, ETeam::Green, i)
				else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::CastleRed))
					SPAWN_UNIT(CastleBP, ETeam::Red, i)
				else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::CastlePurple))
					SPAWN_UNIT(CastleBP, ETeam::Purple, i)
				else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::CastleBrown))
					SPAWN_UNIT(CastleBP, ETeam::Brown, i)
				else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::CastleWhite))
					SPAWN_UNIT(CastleBP, ETeam::White, i)
				else if (ColorsNearlyEqual(GameState->CroppedBoardLayout[i], ColorCode::CastleBlack))
					SPAWN_UNIT(CastleBP, ETeam::Black, i)
			}
			GameState->GroundBoard[i]->Team = ETeam::Neutral;
			GameState->GroundBoard[i]->Coordinates = i;
			GameState->GroundBoard[i]->OnRep_Team();
			GameState->GroundBoard[i]->BuildMisc();
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
	GameState->TeamStats.Empty();
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
	GameState->TeamStats.Reserve(GameState->TeamCount);
	for (uint8 i = 0; i < GameState->TeamCount; i++)
	{
		GameState->Gold.Add(StartingGold);
		GameState->CastleHP.Add(StartingCastleHP);
		GameState->CastleHPMax.Add(StartingCastleHPMax);
		GameState->TeamStats[GameState->TeamStats.Emplace()].GDP = StartingGold;
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

#undef SPAWN_UNIT
#undef SPAWN_SPAWN
