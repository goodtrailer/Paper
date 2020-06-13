// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PaperGameState.generated.h"


enum class ETeam : uint8;

USTRUCT(BlueprintType)
struct FTeamSpawns
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
		TArray<class AUnit*> Spawns;
};

UCLASS()
class PAPER_API APaperGameState : public AGameStateBase
{
	GENERATED_BODY()
	
	friend class APaperGameMode;

public:
	APaperGameState();
	void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;
	UFUNCTION(BlueprintCallable)
	int GetBoardWidth() const;
	UFUNCTION(BlueprintCallable)
	int GetBoardHeight() const;
	UFUNCTION(BlueprintCallable)
	class AUnit* GetBoardSpawn(ETeam team, int index) const;
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_EndTurn();
	UFUNCTION()
	void OnRep_Turn();
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_SetGold(ETeam Team, int NewAmount);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_ChangeGold(ETeam Team, int DeltaGold);
	UFUNCTION(BlueprintCallable)
	int GetGold(ETeam Team) const;
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_CheckDeadUnitForLocalPlayerController(AUnit* Unit);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_CheckUpdatedUnitForLocalPlayerController(AUnit* Unit);
	

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<FTeamSpawns> BoardSpawns;
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_Turn)
	int32 Turn = -1;			// force replication when players connect
	UPROPERTY(BlueprintReadWrite, Replicated)
	TArray<class AUnit*> UnitBoard;
	UPROPERTY(BlueprintReadWrite, Replicated)
	TArray<class AUnit*> GroundBoard;
	UPROPERTY(BlueprintReadWrite, Replicated)
	int PassiveIncome;
	uint8 Count;

protected:
	UFUNCTION()
	void OnRep_Gold();

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_Gold)
	TArray<int> Gold;		// stored on game state and not player state: easily accessible by game mode, and it must be remembered if players reconnect
	UPROPERTY(Replicated)
	int BoardWidth;
	UPROPERTY(Replicated)
	int BoardHeight;
};
