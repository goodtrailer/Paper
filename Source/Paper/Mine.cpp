// Fill out your copyright notice in the Description page of Project Settings.


#include "Mine.h"

void AMine::BuildMisc_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Called AMine::BuildMisc_Implementation()."))
}

AMine::AMine()
{
	PrimaryActorTick.bCanEverTick = false;
}