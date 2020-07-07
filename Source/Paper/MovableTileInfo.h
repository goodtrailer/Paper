// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "MovableTileInfo.generated.h"

enum class EDirection : uint8;

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
