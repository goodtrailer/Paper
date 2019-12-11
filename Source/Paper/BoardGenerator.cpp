// Fill out your copyright notice in the Description page of Project Settings.

#include "BoardGenerator.h"
#include "Engine/World.h"
#include "Engine/Texture2D.h"


ABoardGenerator::ABoardGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
}

ABoardGenerator::~ABoardGenerator()
{
	delete GroundBoard;
	delete UnitBoard;
}

void ABoardGenerator::BeginPlay()
{
	Super::BeginPlay();
	GameWorld = this->GetWorld();

	BoardLayoutMipmap = &BoardLayoutTexture->PlatformData->Mips[0];
	BoardLayoutColorArray = static_cast<FColor*>(BoardLayoutMipmap->BulkData.Lock(LOCK_READ_ONLY));
	{
		int BoardLayoutWidth = BoardLayoutMipmap->SizeX;

		{
			int i = 0;
			for (int x = 0; x < BoardLayoutWidth; x++)
				for (int y = 0; y < BoardLayoutMipmap->SizeY; y++)
					if (ColorsNearlyEqual(ColorCode::Bounds, BoardLayoutColorArray[x + y * BoardLayoutWidth]))
					{
						BoardLayoutBounds[i][0] = x; BoardLayoutBounds[i][1] = y;
						i++;
					}
		}
		BoardLayoutBounds[0][0]++; BoardLayoutBounds[0][1]++; BoardLayoutBounds[1][0]--; BoardLayoutBounds[1][1]--; // crop unused outline around playable board
		BoardWidth = BoardLayoutBounds[1][0] - BoardLayoutBounds[0][0] + 1;
		BoardHeight = BoardLayoutBounds[1][1] - BoardLayoutBounds[0][1] + 1;
		GroundBoard = new AGround*[BoardWidth * BoardHeight];
		UnitBoard = new AUnit*[BoardWidth * BoardHeight];

		int CurrentSpawnToRegister[2] = { 0,0 };
		for (int x = BoardLayoutBounds[0][0]; x <= BoardLayoutBounds[1][0]; x++)
			for (int y = BoardLayoutBounds[0][1]; y <= BoardLayoutBounds[1][1]; y++)
			{
				FVector SpawnLocation = FVector((x - BoardLayoutBounds[0][0]) * 200, (y - BoardLayoutBounds[0][1]) * 200, 0);
				FRotator SpawnRotation = FRotator(0, 0, 0);
				FColor CurrentColor = BoardLayoutColorArray[x + y * BoardLayoutWidth];
				int CurrentBoardCoordinates = x - BoardLayoutBounds[0][0] + (y - BoardLayoutBounds[0][1]) * BoardWidth;
				int CurrentBoardLayoutCoordinates = x + y * BoardLayoutWidth;

				//if not hole, then process, otherwise just skip and never generate
				if (!ColorsNearlyEqual(CurrentColor, ColorCode::Hole))
				{
					// spawn ground, because the board should have ground underneath regardless
					GroundBoard[CurrentBoardCoordinates] = GameWorld->SpawnActor<AGround>(GroundBP, SpawnLocation, SpawnRotation);
					
					// if normal ground
					if (ColorsNearlyEqual(CurrentColor, ColorCode::Ground))
						for (unsigned char q = FCardinal::Up; q <= FCardinal::Left; q++)
							GroundBoard[CurrentBoardCoordinates]->bIsCollidable.Set(q, false);
					
					// if directional ground
					else if (ColorsNearlyEqual(CurrentColor, ColorCode::OneWayU))
						GroundBoard[CurrentBoardCoordinates]->bIsCollidable.Set(FCardinal::Up, true);
					else if (ColorsNearlyEqual(CurrentColor, ColorCode::OneWayR))
						GroundBoard[CurrentBoardCoordinates]->bIsCollidable.Set(FCardinal::Right, true);
					else if (ColorsNearlyEqual(CurrentColor, ColorCode::OneWayD))
						GroundBoard[CurrentBoardCoordinates]->bIsCollidable.Set(FCardinal::Down, true);
					else if (ColorsNearlyEqual(CurrentColor, ColorCode::OneWayL))
						GroundBoard[CurrentBoardCoordinates]->bIsCollidable.Set(FCardinal::Left, true);
				

					else if (ColorsNearlyEqual(CurrentColor, ColorCode::OneWayUL))
					{
						GroundBoard[CurrentBoardCoordinates]->bIsCollidable.Set(FCardinal::Up, true);
						GroundBoard[CurrentBoardCoordinates]->bIsCollidable.Set(FCardinal::Left, true);
					}
					else if (ColorsNearlyEqual(CurrentColor, ColorCode::OneWayUR))
					{
						GroundBoard[CurrentBoardCoordinates]->bIsCollidable.Set(FCardinal::Up, true);
						GroundBoard[CurrentBoardCoordinates]->bIsCollidable.Set(FCardinal::Right, true);
						
					}
					else if (ColorsNearlyEqual(CurrentColor, ColorCode::OneWayDR))
					{
						GroundBoard[CurrentBoardCoordinates]->bIsCollidable.Set(FCardinal::Down, true);
						GroundBoard[CurrentBoardCoordinates]->bIsCollidable.Set(FCardinal::Right, true);
					}
					else if (ColorsNearlyEqual(CurrentColor, ColorCode::OneWayDL))
					{
						GroundBoard[CurrentBoardCoordinates]->bIsCollidable.Set(FCardinal::Down, true);
						GroundBoard[CurrentBoardCoordinates]->bIsCollidable.Set(FCardinal::Left, true);
					}	
					
					else
					{

						// if ground not explicitly normal nor directional, then the ground underneath is normal
						for (unsigned char i = 0; i < 4; i++)
							GroundBoard[CurrentBoardCoordinates]->bIsCollidable.Set(i, false);

						// surface layer now, so spawn 200 units up (size of a block)
						SpawnLocation.Z += 200;

						if (ColorsNearlyEqual(CurrentColor, ColorCode::Wall))
							UnitBoard[CurrentBoardCoordinates] = GameWorld->SpawnActor<AUnit>(WallBP, SpawnLocation, SpawnRotation);
						else if (ColorsNearlyEqual(CurrentColor, ColorCode::Mine))
							UnitBoard[CurrentBoardCoordinates] = GameWorld->SpawnActor<AUnit>(MineBP, SpawnLocation, SpawnRotation);
						else if (ColorsNearlyEqual(CurrentColor, ColorCode::SpawnGreen))
						{
							BoardSpawn[0][CurrentSpawnToRegister[0]] = CurrentBoardCoordinates;
							CurrentSpawnToRegister[0]++;
							
							UE_LOG(LogTemp, Display, TEXT("Green Spawn: (%d, %d)"), CurrentBoardCoordinates % BoardWidth, CurrentBoardCoordinates / BoardWidth);
						}
						else if (ColorsNearlyEqual(CurrentColor, ColorCode::SpawnRed))
						{
							BoardSpawn[1][CurrentSpawnToRegister[1]] = CurrentBoardCoordinates;
							CurrentSpawnToRegister[1]++;
							UE_LOG(LogTemp, Display, TEXT("Red Spawn: (%d, %d)"), CurrentBoardCoordinates % BoardWidth, CurrentBoardCoordinates / BoardWidth);
						}
					}
					
					GroundBoard[CurrentBoardCoordinates]->BuildMisc();
				}
			}
	}
	BoardLayoutMipmap->BulkData.Unlock();
	UE_LOG(LogTemp, Display, TEXT("BoardLayoutBounds are (%d, %d), (%d, %d)"), BoardLayoutBounds[0][0], BoardLayoutBounds[0][1], BoardLayoutBounds[1][0], BoardLayoutBounds[1][1]);
}



bool ABoardGenerator::ColorsNearlyEqual(FColor a, FColor b)
{
	if ((FMath::Abs(a.R - b.R) + FMath::Abs(a.G - b.G) + FMath::Abs(a.B - b.B))/255.f < ColorsNearlyEqualThreshold)
		return true;
	else
		return false;
}

