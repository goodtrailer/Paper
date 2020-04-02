// Fill out your copyright notice in the Description page of Project Settings.

#include "BoardGenerator.h"
#include "UnrealNetwork.h"
#include "Engine/World.h"
#include "Engine/Texture2D.h"


ABoardGenerator::ABoardGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;	
}

ABoardGenerator::~ABoardGenerator()
{
	delete[] UnitBoard;
	delete[] GroundBoard;
}

void ABoardGenerator::BeginPlay()
{
	Super::BeginPlay();
	GenerateBoard();
	Turn = static_cast<uint8>(ETeam::TeamGreen);
}

bool ABoardGenerator::SpawnUnit(ETeam team, TSubclassOf<AUnit> type)
{
	for (int i = 0; i < 2; i++)
		if (UnitBoard[BoardSpawn[static_cast<int>(team)][i]->Coordinates] == nullptr)
		{
			UnitBoard[BoardSpawn[static_cast<int>(team)][i]->Coordinates] = GetWorld()->SpawnActor<AUnit>(type, FVector((BoardSpawn[static_cast<int>(team)][i]->Coordinates % BoardWidth) * 200, BoardSpawn[static_cast<int>(team)][i]->Coordinates / BoardWidth * 200, 200), FRotator(0.f));
			UnitBoard[BoardSpawn[static_cast<int>(team)][i]->Coordinates]->Build(team);
			UnitBoard[BoardSpawn[static_cast<int>(team)][i]->Coordinates]->UnitBoard = UnitBoard;
			UnitBoard[BoardSpawn[static_cast<int>(team)][i]->Coordinates]->Coordinates = BoardSpawn[static_cast<int>(team)][i]->Coordinates;
			return true;
		}
	return false;
}



void ABoardGenerator::Server_Move_Implementation(int start, int destination, uint8 energyLeft)
{
	if (UnitBoard[destination] == nullptr /* && check if destination is within range */)
	{
		if (IsRunningDedicatedServer())
			Move(start, destination, energyLeft);
		Multicast_Move(start, destination, energyLeft);
	}
}

bool ABoardGenerator::Server_Move_Validate(int start, int destination, uint8 energyLeft)
{
	return true;
}

void ABoardGenerator::Multicast_Move_Implementation(int start, int destination, uint8 energyLeft)
{
	Move(start, destination, energyLeft);
}

void ABoardGenerator::Move(int start, int destination, uint8 energyLeft)
{
	UnitBoard[destination] = UnitBoard[start];
	UnitBoard[start] = nullptr;
	UnitBoard[destination]->Coordinates = destination;
	UnitBoard[destination]->SetActorLocation(FVector(destination % BoardWidth * 200, destination / BoardWidth * 200, UnitBoard[destination]->GetActorLocation().Z));
	UnitBoard[destination]->Energy = energyLeft;
}



void ABoardGenerator::GenerateBoard()
{
	BoardLayoutMipmap = &BoardLayoutTexture->PlatformData->Mips[0];
	BoardLayoutColorArray = static_cast<FColor*>(BoardLayoutMipmap->BulkData.Lock(LOCK_READ_ONLY));

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
	GroundBoard = new AUnit*[BoardWidth * BoardHeight];
	UnitBoard = new AUnit*[BoardWidth * BoardHeight];
	for (int i = 0; i < BoardWidth * BoardHeight; i++)
	{
		UnitBoard[i] = nullptr;
		GroundBoard[i] = nullptr;
	}

	int CurrentSpawnToRegister[2] = { 0,0 };
	for (int x = BoardLayoutBounds[0][0]; x <= BoardLayoutBounds[1][0]; x++)
		for (int y = BoardLayoutBounds[0][1]; y <= BoardLayoutBounds[1][1]; y++)
		{
			FVector SpawnLocation((x - BoardLayoutBounds[0][0]) * 200, (y - BoardLayoutBounds[0][1]) * 200, 0);
			FRotator SpawnRotation(0, 0, 0);
			FColor CurrentColor = BoardLayoutColorArray[x + y * BoardLayoutWidth];
			int CurrentBoardCoordinates = x - BoardLayoutBounds[0][0] + (y - BoardLayoutBounds[0][1]) * BoardWidth;
			int CurrentBoardLayoutCoordinates = x + y * BoardLayoutWidth;

			//if not hole, then process, otherwise just skip and never generate
			if (!ColorsNearlyEqual(CurrentColor, ColorCode::Hole))
			{
				// spawn ground, because the board should have ground underneath regardless
				GroundBoard[CurrentBoardCoordinates] = GetWorld()->SpawnActor<AUnit>(GroundBP, SpawnLocation, SpawnRotation);

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
						UnitBoard[CurrentBoardCoordinates] = GetWorld()->SpawnActor<AUnit>(WallBP, SpawnLocation, SpawnRotation);
					else if (ColorsNearlyEqual(CurrentColor, ColorCode::Mine))
						UnitBoard[CurrentBoardCoordinates] = GetWorld()->SpawnActor<AUnit>(MineBP, SpawnLocation, SpawnRotation);
					else if (ColorsNearlyEqual(CurrentColor, ColorCode::SpawnGreen))
					{
						BoardSpawn[0][CurrentSpawnToRegister[0]] = GetWorld()->SpawnActor<AUnit>(SpawnBP, SpawnLocation, SpawnRotation);
						BoardSpawn[0][CurrentSpawnToRegister[0]]->Coordinates = CurrentBoardCoordinates;
						BoardSpawn[0][CurrentSpawnToRegister[0]]->Team = ETeam::TeamGreen;
						BoardSpawn[0][CurrentSpawnToRegister[0]]->Build(ETeam::TeamGreen);
						CurrentSpawnToRegister[0]++;

						UE_LOG(LogTemp, Display, TEXT("Green Spawn: (%d, %d)"), CurrentBoardCoordinates % BoardWidth, CurrentBoardCoordinates / BoardWidth);
					}
					else if (ColorsNearlyEqual(CurrentColor, ColorCode::SpawnRed))
					{
						BoardSpawn[1][CurrentSpawnToRegister[1]] = GetWorld()->SpawnActor<AUnit>(SpawnBP, SpawnLocation, SpawnRotation);
						BoardSpawn[1][CurrentSpawnToRegister[1]]->Coordinates = CurrentBoardCoordinates;
						BoardSpawn[1][CurrentSpawnToRegister[1]]->Team = ETeam::TeamRed;
						BoardSpawn[1][CurrentSpawnToRegister[1]]->Build(ETeam::TeamRed);
						CurrentSpawnToRegister[1]++;
						UE_LOG(LogTemp, Display, TEXT("Red Spawn: (%d, %d)"), CurrentBoardCoordinates % BoardWidth, CurrentBoardCoordinates / BoardWidth);
					}
					
					//TODO: when all remaining colors are implemented, remove if statement for performance (at the moment the only remaining are bases)
					if (UnitBoard[CurrentBoardCoordinates] != nullptr)
					{
						UnitBoard[CurrentBoardCoordinates]->Coordinates = CurrentBoardCoordinates;
						UnitBoard[CurrentBoardCoordinates]->Build(ETeam::TeamNeutral);
						UnitBoard[CurrentBoardCoordinates]->UnitBoard = UnitBoard;
					}
				}
				GroundBoard[CurrentBoardCoordinates]->Build(ETeam::TeamNeutral);
				GroundBoard[CurrentBoardCoordinates]->Coordinates = CurrentBoardCoordinates;
				GroundBoard[CurrentBoardCoordinates]->GroundBoard = GroundBoard;
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

