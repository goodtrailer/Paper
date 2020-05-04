// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Unit.h"
#include "TextureResource.h"
#include "Engine/Texture2D.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardGenerator.generated.h"


UCLASS()
class PAPER_API ABoardGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	ABoardGenerator();
	~ABoardGenerator();
	bool SpawnUnit(ETeam team, TSubclassOf<AUnit> type);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Move(int start, int destination, uint8 energyLeft);

protected:
	virtual void BeginPlay() override;
	void GenerateBoard();
	bool ColorsNearlyEqual(FColor a, FColor b);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Move(int start, int destination, uint8 energyLeft);
	void Move(int start, int destination, uint8 energyLeft	);

public:
	//BoardSpawn[team color (green ~ 0, red ~ 1)][spawn number (there can be multiple spawn locations)]
	AUnit* BoardSpawn[2][2];
	uint16 Turn;

	UPROPERTY(EditAnywhere, Category = "Tile Blueprints")
		TSubclassOf<AUnit> GroundBP;
	UPROPERTY(EditAnywhere, Category = "Tile Blueprints")
		TSubclassOf<AUnit> WallBP;
	UPROPERTY(EditAnywhere, Category = "Tile Blueprints")
		TSubclassOf<AUnit> MineBP;
	UPROPERTY(EditAnywhere, Category = "Tile Blueprints")
		TSubclassOf<AUnit> SpawnBP;

	AUnit** GroundBoard;
	AUnit** UnitBoard;
	UPROPERTY(BlueprintReadOnly)
		int BoardWidth;
	UPROPERTY(BlueprintReadOnly)
		int BoardHeight;
private:
	UPROPERTY(EditAnywhere)
	UTexture2D* BoardLayoutTexture;
	FTexture2DMipMap* BoardLayoutMipmap;
	FColor* BoardLayoutColorArray;
	int BoardLayoutBounds[2][2];
	

	UPROPERTY(EditAnywhere, Category="Misc")
	float ColorsNearlyEqualThreshold;
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

