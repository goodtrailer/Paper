// Copyright (c) 2019–2020 Alden Wu

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
	UFUNCTION(BlueprintNativeEvent)
	void Build(ETeam DesiredTeam);
	UFUNCTION(BlueprintNativeEvent)
	void DetermineAttackableTiles(TSet<int>& OutReachableTiles, TSet<int>& OutAttackableTiles) const;
	// This attack function is called on the server, but because server RPC can't be virtual/blueprintnative afaik, there has to be a protected attack func
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Attack(AUnit* Victim);
	// same as Server_Attack comment
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Die();
	// same as Server_Attack comment
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Passive();
	UFUNCTION()
	void OnRep_Coordinates();
	UFUNCTION(BlueprintNativeEvent)
	int GetCost();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	class UTexture2D* GetIcon();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	class UMaterialInterface* GetMaterial();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;
	// on the bright side, blueprintcallable ufunctions can be virtual
	UFUNCTION(BlueprintCallable)
	virtual uint8 GetHP() const;
	UFUNCTION(BlueprintCallable)
	virtual uint8 GetHPMax() const;
	UFUNCTION(BlueprintCallable)
	virtual void SetHP(uint8 a);				// im gonna kiss myself for doing this encapsulation, amazing foresight past me, because this is perfect for overriding in base/castle class.
	UFUNCTION(BlueprintCallable)
	uint8 GetEnergyMax() const;


	UPROPERTY(ReplicatedUsing = OnRep_Team, VisibleAnywhere, BlueprintReadWrite, Category = "Meta")
	ETeam Team = static_cast<ETeam>(-1);		// force replication
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Meta")
	bool bIsTargetable;
	UPROPERTY(ReplicatedUsing = OnRep_Coordinates, VisibleAnywhere, BlueprintReadWrite, Category = "Meta")
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
	// Attack should never be manually called, it should only be called on the server through Server_Attack. It's done this way because RPCs can't be virtual AFAIK.
	UFUNCTION(BlueprintNativeEvent)
	void Attack(AUnit* Victim);
	// Same as Attack function comment.
	UFUNCTION(BlueprintNativeEvent)
	void Die();
	// Same as Attack function comment.
	UFUNCTION(BlueprintNativeEvent)
	void Passive();
	UFUNCTION(BlueprintCallable)
	void BuildMisc(bool bTargetable, FCardinal bCollidable, ETeam DesiredTeam);
	UFUNCTION()
	void OnRep_Team();
	UFUNCTION()
	void OnRep_RecordedStat();
	
	UPROPERTY(ReplicatedUsing = OnRep_RecordedStat, EditAnywhere, Category = "Stats", DisplayName = "Max Energy")
	uint8 EnergyMax;
	UPROPERTY(ReplicatedUsing = OnRep_RecordedStat, EditAnywhere, Category = "Stats", DisplayName = "Starting HP")
	uint8 HP;
	UPROPERTY(ReplicatedUsing = OnRep_RecordedStat, EditAnywhere, Category = "Stats", DisplayName = "Max HP")
	uint8 HPMax;
};
