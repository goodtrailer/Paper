// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "PaperEnums.generated.h"


UENUM(BlueprintType)
enum class ETeam : uint8
{
	Green,
	Red,
	Purple,
	Brown,
	White,
	Black,
	Neutral
};

UENUM(BlueprintType)
enum class EType : uint8
{
	Unit,			//0
	Wall,
	Spawn,
	Ground,
	Mine,
	Knight,			//5
	Castle,
	Miner


					//10
};

UENUM(BlueprintType)
enum class ERangeType : uint8
{
	Normal,
	Square
};

UENUM(BlueprintType)
enum class EDirection : uint8
{
	Up,
	Right,
	Down,
	Left
};

UENUM(BlueprintType)
enum class EStatus : uint8
{
	Dead,			// Team is valid according to TeamCount, and is currently dead.
	Alive,			// Team is valid according to TeamCount, and is currently taken.
	Open			// Team is valid according to TeamCount, but isn't taken yet.
};
