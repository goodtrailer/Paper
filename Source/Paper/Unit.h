// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Cardinal.h"
#include "CoreMinimal.h"
#include "Materials/Material.h"
#include "GameFramework/Actor.h"
#include "Unit.generated.h"


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
	TypeUnit		UMETA(DisplayName = "Unit"),
	TypeWall		UMETA(DisplayName = "Wall"),
	TypeSpawn		UMETA(DisplayName = "Spawn"),
	TypeGround		UMETA(DisplayName = "Ground"),
	TypeMine		UMETA(DisplayName = "Mine"),
	TypeKnight		UMETA(DisplayName = "Knight")
};

UCLASS()
class PAPER_API AUnit : public AActor
{
	GENERATED_BODY()

public:
	AUnit();
	UFUNCTION(BlueprintNativeEvent)
	void Build(ETeam team);
	virtual void Build_Implementation(ETeam team);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ETeam Team;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FCardinal bIsCollidable;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EType Type;
	UPROPERTY(BlueprintReadWrite)
	bool bIsTargetable;
	int Coordinates;

	TArray<AUnit*> UnitBoard;
	TArray<AUnit*> GroundBoard;

	uint8 GetHPMax();
	uint8 GetHP();
	void SetHP(uint8 a);


protected:
	virtual void Passive();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	uint8 HP;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	uint8 HPMax;
};