// Fill out your copyright notice in the Description page of Project Settings.


#include "Wall.h"

void AWall::BuildMisc_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Called BuildMisc_Implementation()."))
}

AWall::AWall()
{
	PrimaryActorTick.bCanEverTick = false;
	Name = "Wall";
}