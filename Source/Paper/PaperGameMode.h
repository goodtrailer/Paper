// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Unit.h"
#include "PaperGameState.h"
#include "TextureResource.h"
#include "Engine/Texture2D.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PaperGameMode.generated.h"

UCLASS()
class PAPER_API APaperGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	APaperGameMode();
	virtual void PostLogin(APlayerController*) override;
	virtual void BeginPlay() override;

protected:
	void GenerateBoard();
	bool ColorsNearlyEqual(FColor a, FColor b);
	
	UPROPERTY(EditAnywhere, Category = "Tile Blueprints")
		TSubclassOf<AUnit> GroundBP;
	UPROPERTY(EditAnywhere, Category = "Tile Blueprints")
		TSubclassOf<AUnit> WallBP;
	UPROPERTY(EditAnywhere, Category = "Tile Blueprints")
		TSubclassOf<AUnit> MineBP;
	UPROPERTY(EditAnywhere, Category = "Tile Blueprints")
		TSubclassOf<AUnit> SpawnBP;

	UPROPERTY(EditAnywhere)
		UTexture2D* BoardLayoutTexture;
	UPROPERTY(EditAnywhere, Category = "Misc")
		float ColorsNearlyEqualThreshold;
	
	APaperGameState* GameState;

	int Count;
};

namespace ColorCode
{
	const FColor SpawnGreen = FColor(0, 127, 14);
	const FColor SpawnRed = FColor(127, 0, 0);
	const FColor BaseGreen = FColor(0, 254, 33);
	const FColor BaseRed = FColor(254, 0, 0);
	const FColor Wall = FColor(0, 148, 255);
	const FColor Bounds = FColor(0, 0, 0);
	const FColor Mine = FColor(255, 216, 0);
	const FColor Ground = FColor(255, 255, 255);
	const FColor Hole = FColor(255, 106, 0);

	const FColor OneWayUL = FColor(72, 1, 255);
	const FColor OneWayU = FColor(255, 0, 218);
	const FColor OneWayUR = FColor(178, 1, 255);
	const FColor OneWayL = FColor(128, 0, 111);
	const FColor OneWayR = FColor(255, 0, 110);
	const FColor OneWayDL = FColor(32, 0, 125);
	const FColor OneWayD = FColor(127, 0, 55);
	const FColor OneWayDR = FColor(86, 0, 127);
}