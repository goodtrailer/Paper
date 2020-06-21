// Fill out your copyright notice in the Description page of Project Settings.

#include "Castle.h"
#include "Engine/World.h"
#include "PaperGameState.h"

uint8 ACastle::GetHP() const
{
	if (GetWorld()->GetGameState<APaperGameState>() && GetWorld()->GetGameState<APaperGameState>()->CastleHP.Num() > static_cast<int>(Team))
		return GetWorld()->GetGameState<APaperGameState>()->CastleHP[static_cast<int>(Team)];
	else
		return -1;
}

uint8 ACastle::GetHPMax() const
{
	if (GetWorld()->GetGameState<APaperGameState>() && GetWorld()->GetGameState<APaperGameState>()->CastleHPMax.Num() > static_cast<int>(Team))
		return GetWorld()->GetGameState<APaperGameState>()->CastleHPMax[static_cast<int>(Team)];
	else
		return -1;
}

void ACastle::SetHP(uint8 a)
{
	GetWorld()->GetGameState<APaperGameState>()->CastleHP[static_cast<int>(Team)] = a;
}
