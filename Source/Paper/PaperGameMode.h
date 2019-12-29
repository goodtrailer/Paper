// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PaperGameMode.generated.h"

UCLASS()
class PAPER_API APaperGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	APaperGameMode();
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	uint8 PlayerCount;
};
