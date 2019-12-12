// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "Spawn.generated.h"

/**
 * 
 */
UCLASS()
class PAPER_API ASpawn : public AUnit
{
	GENERATED_BODY()
public:
	int Coordinates;
	UFUNCTION(BlueprintNativeEvent)
	void BuildMisc() override;
	void BuildMisc_Implementation();
};
