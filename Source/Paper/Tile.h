// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

UCLASS()
class PAPER_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	struct bIsCollidable
	{
		bool up;
		bool left;
		bool down;
		bool right;
	};
	bool bIsTargetable;
	uint8_t HP;
	const static uint8_t MaxHP;
	
};
