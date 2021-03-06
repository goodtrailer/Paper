// Copyright (c) 2019�2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "MovableTileInfo.h"
#include "GameFramework/Actor.h"
#include "Unit.generated.h"


enum class ETeam : uint8;
enum class EType : uint8;
enum class ERangeType : uint8;

UCLASS()
class PAPER_API AUnit : public AActor
{
	GENERATED_BODY()

public:
	AUnit();
	UFUNCTION(BlueprintNativeEvent)
	void BuildMisc();				// this func is for anything random and doesnt need to be implemented, i.e. ground and team dependent mats
	UFUNCTION(BlueprintNativeEvent)
	void DetermineAttackableTiles(TSet<int>& OutReachableTiles, TSet<int>& OutAttackableTiles) const;
	UFUNCTION(BlueprintNativeEvent)
	void DetermineMovableTiles(TMap<int, FMovableTileInfo>& OutMovableTiles) const;
	UFUNCTION(BlueprintNativeEvent)
	void Attack(AUnit* Victim);
	UFUNCTION(BlueprintNativeEvent)
	void Die();
	UFUNCTION(BlueprintNativeEvent)
	void Passive();
	
	UFUNCTION()
	void OnRep_Team();
	/*UFUNCTION()
	void OnRep_HP();*/

	UFUNCTION(BlueprintCallable)
	virtual uint8 GetHP() const;
	UFUNCTION(BlueprintCallable)
	virtual void SetHP(uint8 a);
	UFUNCTION(BlueprintCallable)
	virtual uint8 GetHPMax() const;
	
	UFUNCTION(BlueprintCallable)
	FIntPoint GetCoordinatesVector();			// Returns an FIntPoint, which is essentially an FIntVector2, except FIntVector2 does not exist in the API. FIntVector and FIntVector4 are, though, which is strange.
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;

	UPROPERTY(ReplicatedUsing = OnRep_Team, VisibleAnywhere, BlueprintReadWrite, Category = "Meta")
	ETeam Team;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Meta")
	int Coordinates;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Meta")
	EType Type;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meta")
	class UTexture2D* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	int Cost;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FString PassiveString;
	//needs to be signed to allow healing (negative damage). TIL dont spam uint8 everywhere for space efficiency, it just makes things harder to maintain
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Damage;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Stats")
	uint8 Range;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Stats", DisplayName = "Range Type")
	ERangeType RangeType;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Stats", DisplayName = "Starting Energy")
	uint8 Energy;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Stats", DisplayName = "Max Energy", meta = (ClampMin = "0"))
	uint8 EnergyMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meta")
	class UTruncatedPrism* HPPrismMeter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meta", DisplayName = "Auto Assign Material For HP Prism Meter")
	bool bHPPrismMeterAutoMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meta")
	bool bIsTargetable;

protected:
	// for castles
	UPROPERTY(Replicated/*Using = OnRep_HP*/, EditAnywhere, Category = "Stats", DisplayName = "Starting HP")
	uint8 HP;
	UPROPERTY(Replicated/*Using = OnRep_HP*/, EditAnywhere, Category = "Stats", DisplayName = "Max HP", meta = (ClampMin = "1"))
	uint8 HPMax;
};
