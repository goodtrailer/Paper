// Copyright (c) 2019–2020 Alden Wu

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
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateTeam(ETeam Team);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ToggleMenu();
};
