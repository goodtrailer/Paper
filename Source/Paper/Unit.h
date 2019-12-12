// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tile.h"
#include "Unit.generated.h"


UCLASS()
class PAPER_API AUnit : public ATile
{
	GENERATED_BODY()

public:
	void BuildMisc() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	uint8 Team;
	enum ETeam : unsigned char
	{
		TeamGreen = 0, TeamRed, TeamNeutral
	};

protected:
	virtual void Passive();
};
