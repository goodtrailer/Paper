// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Unit.h"
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PaperGameState.generated.h"


USTRUCT()
struct FTeamSpawns
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
		TArray<AUnit*> Spawns;
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
		AUnit* GetBoardSpawn(ETeam team, int index) const;

	UPROPERTY(Replicated)
		TArray<FTeamSpawns> BoardSpawns;
	UPROPERTY(Replicated)
	uint16 Turn;
	UPROPERTY(Replicated)
	TArray<AUnit*> UnitBoard;
	UPROPERTY(Replicated)
	TArray<AUnit*> GroundBoard;

protected:
	UPROPERTY(Replicated)
	int BoardWidth;
	UPROPERTY(Replicated)
	int BoardHeight;
};
