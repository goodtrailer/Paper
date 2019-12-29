// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "Knight.generated.h"

UCLASS()
class PAPER_API AKnight : public AUnit
{
	GENERATED_BODY()
public:
	AKnight();
	UFUNCTION(BlueprintNativeEvent)
	void BuildMisc() override;
	void BuildMisc_Implementation();
};
