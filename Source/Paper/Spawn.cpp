// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawn.h"

void ASpawn::BuildMisc_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ASpawn::BuildMisc_Implementation() called."))
}

ASpawn::ASpawn()
{
	PrimaryActorTick.bCanEverTick = false;
	Name = "Spawn";
}