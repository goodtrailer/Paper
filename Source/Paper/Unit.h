// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Cardinal.h"
#include "CoreMinimal.h"
#include "Materials/Material.h"
#include "GameFramework/Actor.h"
#include "Unit.generated.h"


UCLASS()
class PAPER_API AUnit : public AActor
{
	GENERATED_BODY()

public:
	AUnit();
	virtual void BuildMisc();
	bool GetIsTargetable();
	void SetIsTargetable(bool a);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	uint8 Team;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FCardinal bIsCollidable;
	UPROPERTY(VisibleAnywhere)
	FString Name;

	TArray<AUnit*> UnitBoard;
	TArray<AUnit*> GroundBoard;

	uint8 GetHPMax();
	uint8 GetHP();
	void SetHP(uint8 a);

	enum ETeam : unsigned char
	{
		TeamGreen = 0, TeamRed, TeamNeutral
	};


protected:
	virtual void Passive();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsTargetable;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	uint8 HP;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	uint8 HPMax;
};