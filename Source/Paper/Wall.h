// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "Wall.generated.h"

/**
 * 
 */
UCLASS()
class PAPER_API AWall : public AUnit
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	void BuildMisc() override;
	void BuildMisc_Implementation();
};
