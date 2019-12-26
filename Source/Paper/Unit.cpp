// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit.h"

void AUnit::Passive() { }

void AUnit::BuildMisc()
{
	Team = AUnit::TeamNeutral;
	bIsCollidable.Set(FCardinal::Up, true);
	bIsCollidable.Set(FCardinal::Left, true);
	bIsCollidable.Set(FCardinal::Down, true);
	bIsCollidable.Set(FCardinal::Right, true);
	HPMax = 10;
	HP = HPMax;
	bIsTargetable = true;
}

AUnit::AUnit()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	PrimaryActorTick.bCanEverTick = false;
}