// Fill out your copyright notice in the Description page of Project Settings.

#include "BoardGenerator.h"
#include "Engine/World.h"
#include "Engine/Texture2D.h"

// Sets default values
ABoardGenerator::ABoardGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABoardGenerator::BeginPlay()
{
	Super::BeginPlay();
	GameWorld = this->GetWorld();

	BoardLayoutMipmap = &BoardLayoutTexture->PlatformData->Mips[0];
	BoardLayoutColorArray = static_cast<FColor*>(BoardLayoutMipmap->BulkData.Lock(LOCK_READ_ONLY));
	{
		int BoardWidth = BoardLayoutMipmap->SizeX;

		int i = 0;
		for (int x = 0; x < BoardWidth; x++)
			for (int y = 0; y < BoardLayoutMipmap->SizeY; y++)
				if (ColorsNearlyEqual(ColorCode::Bounds, BoardLayoutColorArray[x + y * BoardWidth]))
				{
					BoardLayoutBounds[i][0] = x; BoardLayoutBounds[i][1] = y;
					i++;
					
				}

		BoardLayoutBounds[0][0]++; BoardLayoutBounds[0][1]++; BoardLayoutBounds[1][0]--; BoardLayoutBounds[1][1]--; // crop unused outline around playable board
		
		//TODO: TEMPORARY CODE
		for (int x = BoardLayoutBounds[0][0]; x <= BoardLayoutBounds[1][0]; x++)
			for (int y = BoardLayoutBounds[0][1]; y <= BoardLayoutBounds[1][1]; y++)
				if (ColorsNearlyEqual(BoardLayoutColorArray[x + y * BoardWidth], ColorCode::Ground))
				{

					FVector SpawnLocation = FVector(x * 200, y * 200, 0);
					FRotator SpawnRotation = FRotator(0, 0, 0);
					GameWorld->SpawnActor<ATile>(GroundClass, SpawnLocation, SpawnRotation);
					
				}

	}
	BoardLayoutMipmap->BulkData.Unlock();
	UE_LOG(LogTemp, Display, TEXT("BoardLayoutBounds are (%d, %d), (%d, %d)"), BoardLayoutBounds[0][0], BoardLayoutBounds[0][1], BoardLayoutBounds[1][0], BoardLayoutBounds[1][1]);
}

bool ABoardGenerator::ColorsNearlyEqual(FColor a, FColor b)
{
	
	//UE_LOG(LogTemp, Display, TEXT("%d"), (a.R + a.G + a.B) - (b.R + b.G + b.B))
	//UE_LOG(LogTemp, Display, TEXT("%f"), (a.R + a.G + a.B - b.R - b.G - b.B)/255.f)
	//UE_LOG(LogTemp, Display, TEXT("%f"), ((float)(a.R + a.G + a.B - b.R - b.G - b.B)) / 255.f);
	if (FMath::Abs(((float)(a.R + a.G + a.B - b.R - b.G - b.B)) / 255.f) < ColorsNearlyEqualThreshold)
	{
		UE_LOG(LogTemp, Display, TEXT("%d + %d + %d - %d - %d - %d"), a.R, a.G, a.B, b.R, b.G, b.B)
		UE_LOG(LogTemp, Display, TEXT("true"))
		return true;
	}
	else
	{
		return false;
	}
}

