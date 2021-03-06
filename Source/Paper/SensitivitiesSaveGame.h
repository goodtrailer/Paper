// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SensitivitiesSaveGame.generated.h"


UCLASS()
class PAPER_API USensitivitiesSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = "Sensitivities")
	float RotateSensitivity = 1.f;
	UPROPERTY(BlueprintReadWrite, Category = "Sensitivities")
	float PanSensitivity = 1.f;
	UPROPERTY(BlueprintReadWrite, Category = "Sensitivities")
	float ZoomSensitivity = 1.f;
};

