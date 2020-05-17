// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PaperGameState.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PaperPlayerState.generated.h"

UCLASS()
class PAPER_API APaperPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	APaperPlayerState();
	bool IsTurn();
	void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Meta")
		ETeam Team;
private:
	APaperGameState* GameState;
	
};
