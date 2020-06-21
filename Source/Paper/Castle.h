// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "Castle.generated.h"

UCLASS()
class PAPER_API ACastle : public AUnit
{
	GENERATED_BODY()
	
public:
	uint8 GetHP() const override;
	uint8 GetHPMax() const override;
	void SetHP(uint8 a) override;

	/*
	// uses a pointer to gamestate's castlehp instead of normal hp, completely overriding hp behavior in conjunction with above funcs.
	uint8* CastleHP;
	uint8* CastleHPMax;*/
};
