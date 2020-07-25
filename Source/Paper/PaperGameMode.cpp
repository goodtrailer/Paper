// Copyright (c) 2019–2020 Alden Wu

#include "PaperGameMode.h"
#include "Unit.h"
#include "PaperPlayerController.h"
#include "PaperPlayerState.h"
#include "Castle.h"
#include "PaperGameState.h"
#include "PaperGameInstance.h"
#include "PaperEnums.h"
#include "Engine/Texture2D.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

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
	TArray<AUnit*>& GroundBoard = GameState->GroundBoard;
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
			GroundBoard[i] = GetWorld()->SpawnActor<AUnit>(GroundBP, SpawnLocation, FRotator::ZeroRotator);

			if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::Ground))
				for (uint8 q = 0; q <= 3; q++)
					GroundBoard[i]->bIsCollidable[q] = false;

			// if directional ground
			else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::OneWayU))
				GroundBoard[i]->bIsCollidable[EDirection::Up] = true;
			else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::OneWayR))
				GroundBoard[i]->bIsCollidable[EDirection::Right] = true;
			else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::OneWayD))
				GroundBoard[i]->bIsCollidable[EDirection::Down] = true;
			else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::OneWayL))
				GroundBoard[i]->bIsCollidable[EDirection::Left] = true;
			else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::OneWayUL))
			{
				GroundBoard[i]->bIsCollidable[EDirection::Up] = true;
				GroundBoard[i]->bIsCollidable[EDirection::Left] = true;
			}
			else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::OneWayUR))
			{
				GroundBoard[i]->bIsCollidable[EDirection::Up] = true;
				GroundBoard[i]->bIsCollidable[EDirection::Right] = true;

			}
			else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::OneWayDR))
			{
				GroundBoard[i]->bIsCollidable[EDirection::Down] = true;
				GroundBoard[i]->bIsCollidable[EDirection::Right] = true;
			}
			else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::OneWayDL))
			{
				GroundBoard[i]->bIsCollidable[EDirection::Down] = true;
				GroundBoard[i]->bIsCollidable[EDirection::Left] = true;
			}

			else
			{
				// if ground not explicitly normal nor directional, then the ground underneath is normal
				for (unsigned char q = 0; i < 4; i++)
					GroundBoard[i]->bIsCollidable[q] = false;
				// surface layer now, so spawn 200 units up (size of a block)
				SpawnLocation.Z += 200;

				if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::Wall))
				{
					UnitBoard[i] = GetWorld()->SpawnActor<AUnit>(WallBP, SpawnLocation, FRotator::ZeroRotator);
					UnitBoard[i]->Coordinates = i;
					UnitBoard[i]->Build(ETeam::Neutral);
				}
				else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::Mine))
				{
					UnitBoard[i] = GetWorld()->SpawnActor<AUnit>(MineBP, SpawnLocation, FRotator::ZeroRotator);
					UnitBoard[i]->Coordinates = i;
					UnitBoard[i]->Build(ETeam::Neutral);
				}
				else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::SpawnGreen))
				{
					AUnit* spawn = GetWorld()->SpawnActor<AUnit>(SpawnBP, SpawnLocation, FRotator::ZeroRotator);
					GameState->BoardSpawns[static_cast<uint8>(ETeam::Green)].Spawns.Push(spawn);
					spawn->Coordinates = i;
					spawn->Team = ETeam::Green;
					spawn->Build(ETeam::Green);

					GLog->Logf(TEXT("Green Spawn: (%d, %d)"), i % BoardWidth, i / BoardWidth);
				}
				else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::SpawnRed))
				{
					AUnit* spawn = GetWorld()->SpawnActor<AUnit>(SpawnBP, SpawnLocation, FRotator::ZeroRotator);
					GameState->BoardSpawns[static_cast<uint8>(ETeam::Red)].Spawns.Push(spawn);
					spawn->Coordinates = i;
					spawn->Team = ETeam::Red;
					spawn->Build(ETeam::Red);

					GLog->Logf(TEXT("Red Spawn: (%d, %d)"), i % BoardWidth, i / BoardWidth);
				}
				else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::CastleGreen))
				{
					UnitBoard[i] = GetWorld()->SpawnActor<ACastle>(CastleBP, SpawnLocation, FRotator::ZeroRotator);
					UnitBoard[i]->Coordinates = i;
					UnitBoard[i]->Build(ETeam::Green);
				}
				else if (ColorsNearlyEqual(CroppedBoardLayout[i], ColorCode::CastleRed))
				{
					UnitBoard[i] = GetWorld()->SpawnActor<ACastle>(CastleBP, SpawnLocation, FRotator::ZeroRotator);
					UnitBoard[i]->Coordinates = i;
					UnitBoard[i]->Build(ETeam::Red);
				}
			}
			GroundBoard[i]->Build(ETeam::Neutral);
			GroundBoard[i]->Coordinates = i;
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

	GLog->Logf(TEXT("DESIGNATED NAME: %s"), *DesignatedName);

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
	else
		GLog->Log(TEXT("PS does not exist!"));

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
	ManagedMipMap ManagedBoardLayoutMipMap(&Texture->PlatformData->Mips[0]);
	int BoardLayoutBounds[2][2];
	int& BoardLayoutWidth = ManagedBoardLayoutMipMap->SizeX;
	
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
		FColor Color = ManagedBoardLayoutMipMap.GetColorArray()[i % GameState->BoardWidth + BoardLayoutBounds[0][0] + (i / GameState->BoardWidth + BoardLayoutBounds[0][1]) * BoardLayoutWidth];
		Color.A = UINT8_MAX;
		GameState->CroppedBoardLayout.Add(Color);

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
		GameState->TeamStatuses.Add(EStatus::Open);
	}
	GameState->PassiveIncome = StartingPassiveIncome;

}

void APaperGameMode::ParseBoardLayoutFile(const FString& Filename)
{

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
