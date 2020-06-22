// Copyright (c) 2019–2020 Alden Wu

#include "PaperGameMode.h"
#include "Unit.h"
#include "Castle.h"
#include "PaperGameState.h"
#include "PaperPlayerState.h"
#include "PaperEnums.h"
#include "Engine/Texture2D.h"
#include "EngineUtils.h"
#include "Misc/ScopeLock.h"

APaperGameMode::APaperGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APaperGameMode::BeginPlay()
{
	// BoardSpawns.Num not defined yet
	Super::BeginPlay();
	GameState = GetGameState<APaperGameState>();
	GameState->Turn = static_cast<uint8>(ETeam::TeamGreen);
	//TArray<TArray<ACastle*>> Castles;

#pragma region GenerateBoard

	int& BoardWidth = GameState->BoardWidth;
	int& BoardHeight = GameState->BoardHeight;
	TArray<AUnit*>& UnitBoard = GameState->UnitBoard;
	TArray<AUnit*>& GroundBoard = GameState->GroundBoard;

	struct ManagedMipMap
	{
	public:
		ManagedMipMap(FTexture2DMipMap* Source)
		{
			MipMap = Source;
			ColorArray = reinterpret_cast<FColor*>(MipMap->BulkData.Lock(LOCK_READ_ONLY));
		}

		~ManagedMipMap()
		{
			MipMap->BulkData.Unlock();
		}

		inline FColor* GetColorArray() const
		{
			return ColorArray;
		}

		inline const FTexture2DMipMap* GetMipMap() const
		{
			return MipMap;
		}

		FTexture2DMipMap* operator->()
		{
			return MipMap;
		}

		FTexture2DMipMap& operator*()
		{
			return *MipMap;
		}
	private:
		FTexture2DMipMap* MipMap;
		FColor* ColorArray;
	} ManagedBoardLayoutMipMap(&BoardLayoutTexture->PlatformData->Mips[0]);

	int BoardLayoutBounds[2][2];
	int BoardLayoutWidth = ManagedBoardLayoutMipMap->SizeX;

	{
		int i = 0;
		for (int x = 0; x < BoardLayoutWidth; x++)
			for (int y = 0; y < ManagedBoardLayoutMipMap->SizeY; y++)
				if (ColorsNearlyEqual(ColorCode::Bounds, ManagedBoardLayoutMipMap.GetColorArray()[x + y * BoardLayoutWidth]))
				{
					BoardLayoutBounds[i][0] = x; BoardLayoutBounds[i][1] = y;
					i++;
				}
	}

	BoardLayoutBounds[0][0]++; BoardLayoutBounds[0][1]++; BoardLayoutBounds[1][0]--; BoardLayoutBounds[1][1]--; // crop unused outline around playable board
	BoardWidth = BoardLayoutBounds[1][0] - BoardLayoutBounds[0][0] + 1;
	BoardHeight = BoardLayoutBounds[1][1] - BoardLayoutBounds[0][1] + 1;
	
	for (int i = 0; i < BoardWidth * BoardHeight; i++)
	{
		UnitBoard.Push(nullptr);
		GroundBoard.Push(nullptr);
	}

	for (int x = BoardLayoutBounds[0][0]; x <= BoardLayoutBounds[1][0]; x++)
		for (int y = BoardLayoutBounds[0][1]; y <= BoardLayoutBounds[1][1]; y++)
		{
			FVector SpawnLocation((x - BoardLayoutBounds[0][0]) * 200, (y - BoardLayoutBounds[0][1]) * 200, 0);
			FColor CurrentColor = ManagedBoardLayoutMipMap.GetColorArray()[x + y * BoardLayoutWidth];
			int CurrentBoardCoordinates = x - BoardLayoutBounds[0][0] + (y - BoardLayoutBounds[0][1]) * BoardWidth;
			int CurrentBoardLayoutCoordinates = x + y * BoardLayoutWidth;

			//if not hole, then process, otherwise just skip and never generate
			if (!ColorsNearlyEqual(CurrentColor, ColorCode::Hole))
			{
				// spawn ground, because the board should have ground underneath regardless
				GroundBoard[CurrentBoardCoordinates] = GetWorld()->SpawnActor<AUnit>(GroundBP, SpawnLocation, FRotator::ZeroRotator);

				if (ColorsNearlyEqual(CurrentColor, ColorCode::Ground))
					for (uint8 q = 0; q <= 3; q++)
						GroundBoard[CurrentBoardCoordinates]->bIsCollidable[q] = false;

				// if directional ground
				else if (ColorsNearlyEqual(CurrentColor, ColorCode::OneWayU))
					GroundBoard[CurrentBoardCoordinates]->bIsCollidable[EDirection::Up] = true;
				else if (ColorsNearlyEqual(CurrentColor, ColorCode::OneWayR))
					GroundBoard[CurrentBoardCoordinates]->bIsCollidable[EDirection::Right] = true;
				else if (ColorsNearlyEqual(CurrentColor, ColorCode::OneWayD))
					GroundBoard[CurrentBoardCoordinates]->bIsCollidable[EDirection::Down] = true;
				else if (ColorsNearlyEqual(CurrentColor, ColorCode::OneWayL))
					GroundBoard[CurrentBoardCoordinates]->bIsCollidable[EDirection::Left] = true;
				else if (ColorsNearlyEqual(CurrentColor, ColorCode::OneWayUL))
				{
					GroundBoard[CurrentBoardCoordinates]->bIsCollidable[EDirection::Up] = true;
					GroundBoard[CurrentBoardCoordinates]->bIsCollidable[EDirection::Left] = true;
				}
				else if (ColorsNearlyEqual(CurrentColor, ColorCode::OneWayUR))
				{
					GroundBoard[CurrentBoardCoordinates]->bIsCollidable[EDirection::Up] = true;
					GroundBoard[CurrentBoardCoordinates]->bIsCollidable[EDirection::Right] = true;

				}
				else if (ColorsNearlyEqual(CurrentColor, ColorCode::OneWayDR))
				{
					GroundBoard[CurrentBoardCoordinates]->bIsCollidable[EDirection::Down] = true;
					GroundBoard[CurrentBoardCoordinates]->bIsCollidable[EDirection::Right] = true;
				}
				else if (ColorsNearlyEqual(CurrentColor, ColorCode::OneWayDL))
				{
					GroundBoard[CurrentBoardCoordinates]->bIsCollidable[EDirection::Down] = true;
					GroundBoard[CurrentBoardCoordinates]->bIsCollidable[EDirection::Left] = true;
				}

				else
				{
					// if ground not explicitly normal nor directional, then the ground underneath is normal
					for (unsigned char i = 0; i < 4; i++)
						GroundBoard[CurrentBoardCoordinates]->bIsCollidable[i] = false;
					// surface layer now, so spawn 200 units up (size of a block)
					SpawnLocation.Z += 200;

					if (ColorsNearlyEqual(CurrentColor, ColorCode::Wall))
					{
						UnitBoard[CurrentBoardCoordinates] = GetWorld()->SpawnActor<AUnit>(WallBP, SpawnLocation, FRotator::ZeroRotator);
						UnitBoard[CurrentBoardCoordinates]->Coordinates = CurrentBoardCoordinates;
						UnitBoard[CurrentBoardCoordinates]->Build(ETeam::TeamNeutral);
					}
					else if (ColorsNearlyEqual(CurrentColor, ColorCode::Mine))
					{
						UnitBoard[CurrentBoardCoordinates] = GetWorld()->SpawnActor<AUnit>(MineBP, SpawnLocation, FRotator::ZeroRotator);
						UnitBoard[CurrentBoardCoordinates]->Coordinates = CurrentBoardCoordinates;
						UnitBoard[CurrentBoardCoordinates]->Build(ETeam::TeamNeutral);
					}
					else if (ColorsNearlyEqual(CurrentColor, ColorCode::SpawnGreen))
					{
						while (GameState->BoardSpawns.Num() <= static_cast<uint8>(ETeam::TeamGreen))
							GameState->BoardSpawns.Push(FTeamSpawns());
						AUnit* spawn = GetWorld()->SpawnActor<AUnit>(SpawnBP, SpawnLocation, FRotator::ZeroRotator);
						GameState->BoardSpawns[static_cast<uint8>(ETeam::TeamGreen)].Spawns.Push(spawn);
						spawn->Coordinates = CurrentBoardCoordinates;
						spawn->Team = ETeam::TeamGreen;
						spawn->Build(ETeam::TeamGreen);

						UE_LOG(LogTemp, Display, TEXT("Green Spawn: (%d, %d)"), CurrentBoardCoordinates % BoardWidth, CurrentBoardCoordinates / BoardWidth);
					}
					else if (ColorsNearlyEqual(CurrentColor, ColorCode::SpawnRed))
					{
						while (GameState->BoardSpawns.Num() <= static_cast<uint8>(ETeam::TeamRed))
							GameState->BoardSpawns.Push(FTeamSpawns());
						AUnit* spawn = GetWorld()->SpawnActor<AUnit>(SpawnBP, SpawnLocation, FRotator::ZeroRotator);
						GameState->BoardSpawns[static_cast<uint8>(ETeam::TeamRed)].Spawns.Push(spawn);
						spawn->Coordinates = CurrentBoardCoordinates;
						spawn->Team = ETeam::TeamRed;
						spawn->Build(ETeam::TeamRed);

						UE_LOG(LogTemp, Display, TEXT("Red Spawn: (%d, %d)"), CurrentBoardCoordinates % BoardWidth, CurrentBoardCoordinates / BoardWidth);
					}
					else if (ColorsNearlyEqual(CurrentColor, ColorCode::CastleGreen))
					{
						/*while (Castles.Num() <= static_cast<uint8>(ETeam::TeamGreen))
							Castles.Push(TArray<ACastle*>());
						Castles[static_cast<uint8>(ETeam::TeamGreen)].Add(GetWorld()->SpawnActor<ACastle>(CastleBP, SpawnLocation, FRotator::ZeroRotator));
						UnitBoard[CurrentBoardCoordinates] = Castles[static_cast<uint8>(ETeam::TeamGreen)].Last();*/
						UnitBoard[CurrentBoardCoordinates] = GetWorld()->SpawnActor<ACastle>(CastleBP, SpawnLocation, FRotator::ZeroRotator);
						UnitBoard[CurrentBoardCoordinates]->Coordinates = CurrentBoardCoordinates;
						UnitBoard[CurrentBoardCoordinates]->Build(ETeam::TeamGreen);
					}
					else if (ColorsNearlyEqual(CurrentColor, ColorCode::CastleRed))
					{
						/*while (Castles.Num() <= static_cast<uint8>(ETeam::TeamRed))
							Castles.Push(TArray<ACastle*>());
						Castles[static_cast<uint8>(ETeam::TeamRed)].Add(GetWorld()->SpawnActor<ACastle>(CastleBP, SpawnLocation, FRotator::ZeroRotator));
						UnitBoard[CurrentBoardCoordinates] = Castles[static_cast<uint8>(ETeam::TeamRed)].Last();*/
						UnitBoard[CurrentBoardCoordinates] = GetWorld()->SpawnActor<ACastle>(CastleBP, SpawnLocation, FRotator::ZeroRotator);
						UnitBoard[CurrentBoardCoordinates]->Coordinates = CurrentBoardCoordinates;
						UnitBoard[CurrentBoardCoordinates]->Build(ETeam::TeamRed);
					}
				}
				GroundBoard[CurrentBoardCoordinates]->Build(ETeam::TeamNeutral);
				GroundBoard[CurrentBoardCoordinates]->Coordinates = CurrentBoardCoordinates;
			}
		}
#pragma endregion

	// BoardSpawns.Num is now defined
	for (int i = 0; i < GameState->BoardSpawns.Num(); i++)
	{
		GameState->Gold.Add(StartingGold);
		GameState->CastleHP.Add(StartingCastleHP);
		GameState->CastleHPMax.Add(StartingCastleHPMax);
	}

	/*// just in case some team doesnt have a castle (not sure why that would happen, maybe you want an undefeatable player, who knows? maybe a horde style game mode, idfk)
	for (int i = 0; i < Castles.Num(); i++)
		for (auto& Castle : Castles[i])
		{
			Castle->CastleHP = &GameState->CastleHP[i];
			Castle->CastleHPMax = &GameState->CastleHPMax[i];
		}*/
	GameState->PassiveIncome = StartingPassiveIncome;
}

bool APaperGameMode::ColorsNearlyEqual(FColor a, FColor b)
{
	return ((FMath::Abs(a.R - b.R) + FMath::Abs(a.G - b.G) + FMath::Abs(a.B - b.B)) / 255.f < ColorsNearlyEqualThreshold) ? true : false;
}
