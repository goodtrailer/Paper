// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BoardPreviewUserInterface.generated.h"

UCLASS()
class PAPER_API UBoardPreviewUserInterface : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateBoardPreview(const UTexture2D* BoardTexture);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ResetBoardPreview();
};
