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


		for (int x = BoardLayoutBounds[0][0]; x <= BoardLayoutBounds[1][0]; x++)
			for (int y = BoardLayoutBounds[0][1]; y <= BoardLayoutBounds[1][1]; y++)
			{
				FVector SpawnLocation = FVector((x - BoardLayoutBounds[0][0]) * 200, (y - BoardLayoutBounds[0][1]) * 200, 0);
				FRotator SpawnRotation = FRotator(0, 0, 0);
				FColor CurrentColor = BoardLayoutColorArray[x + y * BoardLayoutWidth];
				int CurrentBoardCoordinates = x - BoardLayoutBounds[0][0] + (y - BoardLayoutBounds[0][1]) * BoardWidth;
				int CurrentBoardLayoutCoordinates = x + y * BoardLayoutWidth;

				if (!ColorsNearlyEqual(CurrentColor, ColorCode::Hole))
				{
					GroundBoard[CurrentBoardCoordinates] = GameWorld->SpawnActor<AGround>(GroundBP, SpawnLocation, SpawnRotation);
					
					if (ColorsNearlyEqual(CurrentColor, ColorCode::Ground))
						for (unsigned char q = FCardinal::Up; q <= FCardinal::Left; q++)
							GroundBoard[CurrentBoardCoordinates]->bIsCollidable.Set(q, false);
							
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
						for (unsigned char i = 0; i < 4; i++)
							GroundBoard[CurrentBoardCoordinates]->bIsCollidable.Set(i, false);

						//PUT OTHER SURFACE LEVEL TILES HERE
					}
					
					GroundBoard[CurrentBoardCoordinates]->GenerateOneWayArrows();
				}
				else
				{
					UE_LOG(LogTemp, Display, TEXT("False on: (%d, %d), Color: (%d, %d, %d)"), x, y, CurrentColor.R, CurrentColor.G, CurrentColor.B)
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

