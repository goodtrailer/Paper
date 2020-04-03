// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Cardinal.h"

#include "CoreMinimal.h"
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

	uint8 GetHPMax();
	uint8 GetHP();
	void SetHP(uint8 a);

	virtual void Passive();


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	ETeam Team;
	UPROPERTY(BlueprintReadWrite)
	FCardinal bIsCollidable;
	int Coordinates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
		bool bIsTargetable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
		EType Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", DisplayName = "Starting Energy")
		uint8 Energy;

	AUnit** UnitBoard;
	AUnit** GroundBoard;


protected:
	UPROPERTY(EditAnywhere, BLueprintReadWrite, Category = "Stats", DisplayName = "Max Energy")
		uint8 EnergyMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats", DisplayName="Starting HP")
		uint8 HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats", DisplayName="Max HP")
		uint8 HPMax;
};