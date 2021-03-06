// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PaperHUD.generated.h"


UCLASS()
class PAPER_API APaperHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	void DrawHUD() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bShowHPBars = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bShowRadiusThresholdCircle = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int CursorRadiusThreshold = 125;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int HPBarLength = 25;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int HPBarHeight = 2;

private:
	void BeginPlay() override;

	class APaperGameState* GameState;
	class APlayerController* PlayerController;
};
