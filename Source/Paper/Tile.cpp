// Fill out your copyright notice in the Description page of Project Settings.
#include "Tile.h"

void ATile::BeginPlay()
{
	Super::BeginPlay();
}

ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATile::BuildMisc()
{
	UE_LOG(LogTemp, Warning, TEXT("Called ATile::BuildMisc()."))
}


void ATile::SetIsTargetable(bool a) { bIsTargetable = a; }

bool ATile::GetIsTargetable() { return bIsTargetable; }

void ATile::SetHP(uint8 a) { HP = a; }

uint8 ATile::GetHP() { return HP; }

uint8 ATile::GetHPMax() { return HPMax; }