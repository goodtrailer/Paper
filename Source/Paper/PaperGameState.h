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
	class AUnit* GetBoardSpawn(ETeam team, int index) const;
	
	UFUNCTION(BlueprintCallable)
	void Defeat(ETeam DefeatedTeam);
	void EndTurn();
	
	void SetGold(ETeam Team, int NewAmount);
	void ChangeGold(ETeam Team, int DeltaGold);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartGameForLocalPlayerController();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_CheckDeadUnitForLocalPlayerController(AUnit* Unit);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RemovePlayerForLocalLobbyUI(const FString& Name);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Message(const FText& Message);
	void CheckUpdatedUnitForLocalPlayerController(AUnit* Unit);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;
	

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<FTeamSpawns> BoardSpawns;
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_Turn)
	int32 Turn;							// Modulus by number of teams in game should return the team that is currently taking their turn.
	UPROPERTY(BlueprintReadWrite, Replicated)
	TArray<class AUnit*> UnitBoard;
	UPROPERTY(BlueprintReadWrite, Replicated)
	TArray<class AUnit*> GroundBoard;
	UPROPERTY(BlueprintReadWrite, Replicated)
	int PassiveIncome;
	UPROPERTY(Replicated)
	TArray<uint8> CastleHP;				// An array of the current HP value for each team's castle.
	UPROPERTY(Replicated)
	TArray<uint8> CastleHPMax;			// An array of the maximum HP value for each team's castle.
	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<EStatus> TeamStatuses;
	UPROPERTY(Replicated, BlueprintReadOnly)
	uint8 TeamCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool bGameStarted;
	
protected:
	UFUNCTION()
	void OnRep_Turn();
	UFUNCTION()
	void OnRep_Gold();
	UFUNCTION()
	void OnRep_CroppedBoardLayout();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Victory(ETeam Team);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Defeat(ETeam Team);

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_Gold)
	TArray<int> Gold;					// stored on game state and not player state: easily accessible by game mode, and it must be remembered if players reconnect
	UPROPERTY(Replicated)
	int BoardWidth;
	UPROPERTY(Replicated)
	int BoardHeight;
	UPROPERTY(ReplicatedUsing = OnRep_CroppedBoardLayout)
	TArray<FColor> CroppedBoardLayout;
};
