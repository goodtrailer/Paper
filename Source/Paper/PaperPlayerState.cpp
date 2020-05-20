// Fill out your copyright notice in the Description page of Project Settings.

#include "PaperPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

bool APaperPlayerState::IsTurn()
{
	return (GameState->Turn % 2 == static_cast<uint8>(Team)) ? true : false;
}

void APaperPlayerState::BeginPlay()
{
	GameState = GetWorld()->GetGameState<APaperGameState>();
}

void APaperPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APaperPlayerState, Team)
}

APaperPlayerState::APaperPlayerState()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	// PrimaryActorTick.bCanEverTick = false;
}
