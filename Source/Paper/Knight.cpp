// Fill out your copyright notice in the Description page of Project Settings.


#include "Knight.h"

AKnight::AKnight()
{
	PrimaryActorTick.bCanEverTick = false;
	Name = "Knight";
}

void AKnight::BuildMisc_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Called BuildMisc_Implementation()."))
}