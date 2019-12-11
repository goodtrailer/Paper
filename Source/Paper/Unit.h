// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tile.h"
#include "Unit.generated.h"


UCLASS()
class PAPER_API AUnit : public ATile
{
	enum ETeam : unsigned char
	{
		TeamNeutral = 0, TeamRed, TeamGreen
	};

	GENERATED_BODY()

protected:
	virtual void Passive();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	uint8 Team;

};
