// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Cardinal.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

UCLASS()
class PAPER_API ATile : public AActor
{
	GENERATED_BODY()
	
public:
	ATile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay();

public:
	UPROPERTY(BlueprintReadWrite)
	FCardinal bIsCollidable;

	uint8 GetHPMax();
	uint8 GetHP();
	void SetHP(uint8 a);

	bool GetIsTargetable();
	void SetIsTargetable(bool a);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsTargetable;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	uint8 HP;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	uint8 HPMax;

};
