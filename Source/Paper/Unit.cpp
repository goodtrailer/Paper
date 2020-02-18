// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit.h"

void AUnit::Passive() { }

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = false;
	Type = EType::TypeUnit;
}

void AUnit::Build_Implementation(ETeam team)
{
	UE_LOG(LogTemp, Warning, TEXT("Build_Implementation"))
}

void AUnit::SetHP(uint8 a) { HP = a; }

uint8 AUnit::GetHP() { return HP; }

uint8 AUnit::GetHPMax() { return HPMax; }

void AUnit::Server_MoveTo_Implementation(int destination, int boardWidth)
{
	//UE_LOG(LogTemp, Display, TEXT("UnitBoard size = %d"), UnitBoard.Num())
	UE_LOG(LogTemp, Display, TEXT("Type = %d, Destination = %d"), Type, destination)
	if (UnitBoard[destination] == nullptr /* && check if destination is within range */)
	{
		if (IsRunningDedicatedServer())
			MoveTo(destination, boardWidth);
		Multicast_MoveTo(destination, boardWidth);
	}
}

bool AUnit::Server_MoveTo_Validate(int destination, int boardWidth)
{
	return true;
}

void AUnit::Multicast_MoveTo_Implementation(int destination, int boardWidth)
{
	MoveTo(destination, boardWidth);
}

void AUnit::MoveTo(int destination, int boardWidth)
{
	UnitBoard[destination] = this;
	UnitBoard[Coordinates] = nullptr;
	Coordinates = destination;
	SetActorLocation(FVector(destination%boardWidth*200, destination/boardWidth*200, GetActorLocation().Z));
}