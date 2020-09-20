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
	Miner,
	Scout,
	Greatshield,
	Archer			//10
};

UENUM(BlueprintType)
enum class ERangeType : uint8
{
	Normal,
	Square
};

UENUM(BlueprintType)
enum class EDirection : uint8			// You can have multiple directions at a time, so it is setup as an enum flag
{										// Other enums are mutually exclusive, so they arent setup as enum flags
	Zero	=	0,
	Up		=	1 << 0,
	Right	=	1 << 1,
	Down	=	1 << 2,
	Left	=	1 << 3
};

inline EDirection operator|(EDirection a, EDirection b)
{
	return static_cast<EDirection>(static_cast<uint8>(a) | static_cast<uint8>(b));
}

inline EDirection operator&(EDirection a, EDirection b)
{
	return static_cast<EDirection>(static_cast<uint8>(a) & static_cast<uint8>(b));
}

inline bool operator!(EDirection a) {
	return a == static_cast<EDirection>(0);
}

inline int EDirectionToDegrees(const EDirection a)
{
	if (a == EDirection::Up)
		return 0;
	else if (a == EDirection::Right)
		return 90;
	else if (a == EDirection::Down)
		return 180;
	else if (a == EDirection::Left)
		return 270;
	else
		return 0;
}

UENUM(BlueprintType)
enum class EStatus : uint8
{
	Dead,			// Team is valid according to TeamCount, and is currently dead.
	Alive,			// Team is valid according to TeamCount, and is currently taken.
	Open			// Team is valid according to TeamCount, but isn't taken yet.
};
