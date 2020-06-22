// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "PaperEnums.generated.h"


UENUM(BlueprintType)
enum class ETeam : uint8
{
	TeamGreen		UMETA(DisplayName = "Green"),
	TeamRed			UMETA(DisplayName = "Red"),
	TeamNeutral		UMETA(DisplayName = "Neutral")
};

UENUM(BlueprintType)
enum class EType : uint8
{
	TypeUnit		UMETA(DisplayName = "Unit"),		//0
	TypeWall		UMETA(DisplayName = "Wall"),
	TypeSpawn		UMETA(DisplayName = "Spawn"),
	TypeGround		UMETA(DisplayName = "Ground"),
	TypeMine		UMETA(DisplayName = "Mine"),
	TypeKnight		UMETA(DisplayName = "Knight"),		//5
	TypeCastle		UMETA(DisplayName = "Castle")
};

UENUM(BlueprintType)
enum class ERangeType : uint8
{
	RangeTypeNormal		UMETA(DisplayName = "Normal"),
	RangeTypeSquare		UMETA(DisplayName = "Square")
};

UENUM(BlueprintType)
enum class EDirection : uint8
{
	Up = 0, Right, Down, Left
};
