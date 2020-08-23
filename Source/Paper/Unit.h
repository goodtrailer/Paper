// Copyright (c) 2019–2020 Alden Wu

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
	void DetermineAttackableTiles(TSet<int>& OutReachableTiles, TSet<int>& OutAttackableTiles) const;
	UFUNCTION(BlueprintNativeEvent)
	void DetermineMovableTiles(TMap<int, FMovableTileInfo>& OutMovableTiles) const;
	UFUNCTION(BlueprintNativeEvent)
	void Attack(AUnit* Victim);
	UFUNCTION(BlueprintNativeEvent)
	void Die();
	UFUNCTION(BlueprintNativeEvent)
	void Passive();
	UFUNCTION(BlueprintNativeEvent)
	int GetCost();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	class UTexture2D* GetIcon();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	class UMaterialInterface* GetMaterial();
	UFUNCTION()
	void OnRep_RecordedStat();
	UFUNCTION()
	void OnRep_Team();
	UFUNCTION()
	void OnRep_HP();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;
	UFUNCTION(BlueprintCallable)
	virtual uint8 GetHP() const;
	UFUNCTION(BlueprintCallable)
	virtual uint8 GetHPMax() const;
	UFUNCTION(BlueprintCallable)
	virtual void SetHP(uint8 a);
	UFUNCTION(BlueprintCallable)
	uint8 GetEnergyMax() const;
	UFUNCTION(BlueprintCallable)
	FIntPoint GetCoordinatesVector();			// Returns an FIntPoint, which is essentially an FIntVector2, except FIntVector2 does not exist in the API. FIntVector and FIntVector4 are, though, which is strange.
	UFUNCTION(BlueprintCallable)
	virtual class UMeshComponent* GetMeshComponent();

	UPROPERTY(ReplicatedUsing = OnRep_Team, VisibleAnywhere, BlueprintReadWrite, Category = "Meta")
	ETeam Team;
	UPROPERTY(ReplicatedUsing = OnRep_RecordedStat, VisibleAnywhere, BlueprintReadWrite, Category = "Meta")
	int Coordinates;
	UPROPERTY(ReplicatedUsing = OnRep_RecordedStat, EditAnywhere, BlueprintReadWrite, Category = "Meta")
	EType Type;
	UPROPERTY(ReplicatedUsing = OnRep_RecordedStat, EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FString PassiveString;
	UPROPERTY(ReplicatedUsing = OnRep_RecordedStat, EditAnywhere, BlueprintReadWrite, Category = "Stats")
	uint8 Damage;
	UPROPERTY(ReplicatedUsing = OnRep_RecordedStat, EditAnywhere, BlueprintReadWrite, Category = "Stats")
	uint8 Range;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Stats", DisplayName = "Range Type")
	ERangeType RangeType;
	UPROPERTY(ReplicatedUsing = OnRep_RecordedStat, EditAnywhere, BlueprintReadWrite, Category = "Stats", DisplayName = "Starting Energy")
	uint8 Energy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meta")
	class UTruncatedPrism* HPPrismMeter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meta", DisplayName = "Auto Assign Material For HP Prism Meter")
	bool bHPPrismMeterAutoMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meta")
	bool bIsTargetable;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_RecordedStat, EditAnywhere, Category = "Stats", DisplayName = "Max Energy", meta = (ClampMin = "1"))
	uint8 EnergyMax;
	UPROPERTY(ReplicatedUsing = OnRep_HP, EditAnywhere, Category = "Stats", DisplayName = "Starting HP")
	uint8 HP;
	UPROPERTY(ReplicatedUsing = OnRep_HP, EditAnywhere, Category = "Stats", DisplayName = "Max HP", meta = (ClampMin = "1"))
	uint8 HPMax;
};
