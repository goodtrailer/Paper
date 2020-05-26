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
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;
	UFUNCTION(BlueprintCallable)
	int GetBoardWidth() const;
	UFUNCTION(BlueprintCallable)
	int GetBoardHeight() const;
	UFUNCTION(BlueprintCallable)
	class AUnit* GetBoardSpawn(ETeam team, int index) const;
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_EndTurn();

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<FTeamSpawns> BoardSpawns;
	UPROPERTY(BlueprintReadWrite, Replicated)
	int32 Turn;
	UPROPERTY(BlueprintReadWrite, Replicated)
	TArray<class AUnit*> UnitBoard;
	UPROPERTY(BlueprintReadWrite, Replicated)
	TArray<class AUnit*> GroundBoard;
	uint8 Count;

protected:
	UPROPERTY(Replicated)
	int BoardWidth;
	UPROPERTY(Replicated)
	int BoardHeight;
};
