// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Tile.h"
#include "Ground.generated.h"

UCLASS()
class PAPER_API AGround : public ATile
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void BuildMisc() override;
	void BuildMisc_Implementation();
	
};

