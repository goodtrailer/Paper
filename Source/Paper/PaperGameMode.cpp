// Fill out your copyright notice in the Description page of Project Settings.


#include "PaperGameMode.h"
#include "Unit.h"
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
	Super::BeginPlay();
	GameState = GetGameState<APaperGameState>();
	GenerateBoard();
	GameState->Turn = static_cast<uint8>(ETeam::TeamGreen);
}

void APaperGameMode::GenerateBoard()
{
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
			ColorArray = reinterpret_cast<const FColor*>(MipMap->BulkData.LockReadOnly());
		}

		~ManagedMipMap()
		{
			MipMap->BulkData.Unlock();
		}

		inline const FColor* GetColorArray() const
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
		const FColor* ColorArray;
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
						UnitBoard[CurrentBoardCoordinates] = GetWorld()->SpawnActor<AUnit>(WallBP, SpawnLocation, FRotator::ZeroRotator);
					else if (ColorsNearlyEqual(CurrentColor, ColorCode::Mine))
						UnitBoard[CurrentBoardCoordinates] = GetWorld()->SpawnActor<AUnit>(MineBP, SpawnLocation, FRotator::ZeroRotator);
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

					//TODO: when all remaining colors are implemented, remove if statement for performance (at the moment the only remaining are bases)
					if (UnitBoard[CurrentBoardCoordinates] != nullptr)
					{
						UnitBoard[CurrentBoardCoordinates]->Coordinates = CurrentBoardCoordinates;
						UnitBoard[CurrentBoardCoordinates]->Build(ETeam::TeamNeutral);
					}
				}
				GroundBoard[CurrentBoardCoordinates]->Build(ETeam::TeamNeutral);
				GroundBoard[CurrentBoardCoordinates]->Coordinates = CurrentBoardCoordinates;
			}
		}
	UE_LOG(LogTemp, Display, TEXT("BoardLayoutBounds are (%d, %d), (%d, %d)"), BoardLayoutBounds[0][0], BoardLayoutBounds[0][1], BoardLayoutBounds[1][0], BoardLayoutBounds[1][1]);
}

bool APaperGameMode::ColorsNearlyEqual(FColor a, FColor b)
{
	return ((FMath::Abs(a.R - b.R) + FMath::Abs(a.G - b.G) + FMath::Abs(a.B - b.B)) / 255.f < ColorsNearlyEqualThreshold) ? true : false;
}

