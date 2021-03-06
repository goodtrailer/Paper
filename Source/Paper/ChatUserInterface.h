// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatUserInterface.generated.h"

/**
 * 
 */
UCLASS()
class PAPER_API UChatUserInterface : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void AddMessage(const FText& Message);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void FocusChatbox();
};
