// Fill out your copyright notice in the Description page of Project Settings.
#include "Tile.h"



ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ATile::BeginPlay()
{
	Super::BeginPlay();
	
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