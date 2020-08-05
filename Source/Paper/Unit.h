// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "Cardinal.h"
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
	void Build(ETeam DesiredTeam);
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
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;
	// on the bright side, blueprintcallable ufunctions can be virtual
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

	UPROPERTY(ReplicatedUsing = OnRep_Team, VisibleAnywhere, BlueprintReadWrite, Category = "Meta")
	ETeam Team;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Meta")
	bool bIsTargetable;
	UPROPERTY(ReplicatedUsing = OnRep_RecordedStat, VisibleAnywhere, BlueprintReadWrite, Category = "Meta")
	int Coordinates;
	UPROPERTY(ReplicatedUsing = OnRep_RecordedStat, EditAnywhere, BlueprintReadWrite, Category = "Meta")
	EType Type;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Meta")
	FCardinal bIsCollidable;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* StaticMeshComponent;

protected:
	UFUNCTION(BlueprintCallable)
	void BuildMisc(bool bTargetable, FCardinal bCollidable, ETeam DesiredTeam);
	UFUNCTION()
	void OnRep_Team();
	
	UPROPERTY(ReplicatedUsing = OnRep_RecordedStat, EditAnywhere, Category = "Stats", DisplayName = "Max Energy")
	uint8 EnergyMax;
	UPROPERTY(ReplicatedUsing = OnRep_RecordedStat, EditAnywhere, Category = "Stats", DisplayName = "Starting HP")
	uint8 HP;
	UPROPERTY(ReplicatedUsing = OnRep_RecordedStat, EditAnywhere, Category = "Stats", DisplayName = "Max HP")
	uint8 HPMax;
};
