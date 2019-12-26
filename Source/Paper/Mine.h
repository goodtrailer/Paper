// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "Mine.generated.h"


UCLASS()
class PAPER_API AMine : public AUnit
{
	GENERATED_BODY()
public:
	AMine();
	UFUNCTION(BlueprintNativeEvent)
	void BuildMisc() override;
	void BuildMisc_Implementation();
};
