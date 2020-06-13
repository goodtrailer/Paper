#pragma once

#include "CoreMinimal.h"
#include "Cardinal.h"
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
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Build(ETeam team);
	UFUNCTION(BlueprintNativeEvent)
	void Passive();
	UFUNCTION()
	void OnRep_Coordinates();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Attack(AUnit* UnitToAttack);
	UFUNCTION(BlueprintNativeEvent)
	int GetCost();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	class UTexture2D* GetIcon();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	class UMaterialInterface* GetMaterial();
	virtual void DetermineAttackableTiles(TSet<int>& OutReachableTiles, TSet<int>& OutAttackableTiles) const;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;
	uint8 GetHPMax() const;
	uint8 GetHP() const;
	void SetHP(uint8);
	void BeginPlay() override;

	UPROPERTY(ReplicatedUsing=OnRep_Team, VisibleAnywhere, BlueprintReadWrite, Category = "Meta")
	ETeam Team;
	UPROPERTY(Replicated, BlueprintReadWrite)
	FCardinal bIsCollidable;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Meta")
	bool bIsTargetable;
	UPROPERTY(ReplicatedUsing=OnRep_RecordedStat, EditAnywhere, BlueprintReadWrite, Category = "Meta")
	EType Type;
	UPROPERTY(ReplicatedUsing=OnRep_RecordedStat, EditAnywhere, BlueprintReadWrite, Category = "Stats", DisplayName = "Attack")
	uint8 Attack;
	UPROPERTY(ReplicatedUsing=OnRep_RecordedStat, EditAnywhere, BlueprintReadWrite, Category = "Stats", DisplayName = "Range")
	uint8 Range;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Stats", DisplayName = "Range Type")
	ERangeType RangeType;
	UPROPERTY(ReplicatedUsing=OnRep_RecordedStat, EditAnywhere, BlueprintReadWrite, Category = "Stats", DisplayName = "Starting Energy")
	uint8 Energy;
	UPROPERTY(ReplicatedUsing=OnRep_Coordinates, BlueprintReadWrite, Category = "Meta")
	int Coordinates;

protected:
	UFUNCTION()
	void OnRep_Team();
	UFUNCTION()
	void OnRep_RecordedStat();
	
	UPROPERTY(ReplicatedUsing=OnRep_RecordedStat, EditAnywhere, BLueprintReadWrite, Category = "Stats", DisplayName = "Max Energy")
	uint8 EnergyMax;
	UPROPERTY(ReplicatedUsing=OnRep_RecordedStat, EditAnywhere, BlueprintReadWrite, Category = "Stats", DisplayName = "Starting HP")
	uint8 HP;
	UPROPERTY(ReplicatedUsing=OnRep_RecordedStat, EditAnywhere, BlueprintReadWrite, Category = "Stats", DisplayName = "Max HP")
	uint8 HPMax;
};
