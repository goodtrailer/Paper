// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PaperUserInterface.generated.h"

UCLASS()
class PAPER_API UPaperUserInterface : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateTurn(bool bIsTurn);
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateSelectedUnit(class AUnit* SelectedUnit);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateGold(int Gold);
};