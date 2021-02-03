// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PaperGameState.generated.h"


enum class ETeam : uint8;
enum class EStatus : uint8;

USTRUCT(BlueprintType)
struct FTeamSpawns
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
		TArray<class AUnit*> Spawns;
};

USTRUCT(BlueprintType)
struct FTeamStats
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		int Kills;
	UPROPERTY(BlueprintReadWrite)
		int Deaths;
	UPROPERTY(BlueprintReadWrite)
		int NetDamage;
	UPROPERTY(BlueprintReadWrite)
		int GDP;
};

UCLASS()
class PAPER_API APaperGameState : public AGameStateBase
{
	GENERATED_BODY()
	
	friend class APaperGameMode;

public:
	APaperGameState();
	
	UFUNCTION(BlueprintCallable)
	int GetBoardWidth() const;
	UFUNCTION(BlueprintCallable)
	int GetBoardHeight() const;
	UFUNCTION(BlueprintCallable)
	int GetGold(ETeam Team) const;
	UFUNCTION(BlueprintCallable)
	float GetTimer(ETeam Team) const;
	UFUNCTION(BlueprintCallable)
	float GetRemainingDelay() const;
	UFUNCTION(BlueprintCallable)
	class AUnit* GetBoardSpawn(ETeam team, int index) const;
	UFUNCTION(BlueprintCallable)
	class AUnit* GetUnit(const FIntPoint& CoordinatesVector);
	
	UFUNCTION(BlueprintCallable)
	void Defeat(ETeam DefeatedTeam);
	void EndTurn();
	
	UFUNCTION(BlueprintCallable)
	void SetGold(ETeam Team, int NewAmount);
	UFUNCTION(BlueprintCallable)
	void ChangeGold(ETeam Team, int DeltaGold);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartGameForLocalPlayerController();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_CheckDeadUnitForLocalPlayerController(AUnit* Unit);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RemovePlayerForLocalLobbyUI(const FString& Name);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Message(const FText& Message);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySound(USoundBase* Sound);
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_UpdateBoardPreview(const TArray<FColor>& CroppedBoardLayout);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<FTeamSpawns> BoardSpawns;
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_Turn)
	int32 Turn;							// Modulus by number of teams in game should return the team that is currently taking their turn.
	UPROPERTY(BlueprintReadWrite, Replicated)
	TArray<class AUnit*> UnitBoard;
	UPROPERTY(BlueprintReadWrite, Replicated)
	TArray<class AGround*> GroundBoard;
	UPROPERTY(BlueprintReadWrite, Replicated)
	int PassiveIncome;
	UPROPERTY(Replicated)
	TArray<uint8> CastleHP;				// An array of the current HP value for each team's castle.
	UPROPERTY(Replicated)
	TArray<uint8> CastleHPMax;			// An array of the maximum HP value for each team's castle.
	UPROPERTY(BlueprintReadWrite, Replicated, VisibleAnywhere)
	TArray<float> TeamTimers;
	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<EStatus> TeamStatuses;
	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<FTeamStats> TeamStats;
	UPROPERTY(Replicated, BlueprintReadOnly)
	uint8 TeamCount;
	UPROPERTY(ReplicatedUsing = OnRep_InitialTimer, BlueprintReadWrite)
	int InitialTimer = 125;
	UPROPERTY(ReplicatedUsing = OnRep_DelayCoefficient, BlueprintReadWrite)
	int DelayCoefficient = 10;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool bGameStarted;
	
protected:
	UFUNCTION()
	void OnRep_Turn();
	UFUNCTION()
	void OnRep_InitialTimer();
	UFUNCTION()
	void OnRep_DelayCoefficient();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Victory(ETeam Team);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Defeat(ETeam Team);

	UPROPERTY(Replicated, BlueprintReadWrite)
	TArray<int> Gold;					// stored on game state and not player state: easily accessible by game mode, and it must be remembered if players reconnect
	UPROPERTY(Replicated)
	int BoardWidth;
	UPROPERTY(Replicated)
	int BoardHeight;
	UPROPERTY(Replicated)
	float TurnStartTime;
	UPROPERTY(Replicated)
	int CurrentDelay;
	FTimerHandle TurnTimerHandle;
	AUnit* CurrentTurnCastle;		// any arbitrary castle that the player whose turn it currently is owns
};
