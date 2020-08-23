// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "PaperEnums.h"
#include "MovableTileInfo.generated.h"

USTRUCT(BlueprintType)
struct FMovableTileInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	uint8 EnergyLeft;
	UPROPERTY(BlueprintReadWrite)
	int SourceTileCoordinates;
	UPROPERTY(BlueprintReadWrite)
	EDirection DirectionToSourceTile;
};
