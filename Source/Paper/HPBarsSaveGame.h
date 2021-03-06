// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "HPBarsSaveGame.generated.h"


UCLASS()
class PAPER_API UHPBarsSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = "HP Bar")
	int HPBarHeight = 2;
	UPROPERTY(BlueprintReadWrite, Category = "HP Bar")
	int HPBarLength = 25;
	UPROPERTY(BlueprintReadWrite, Category = "HP Bar")
	int HPBarShowRadius = 125;
};
