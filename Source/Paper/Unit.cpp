// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit.h"

void AUnit::Passive_Implementation()
{
	Energy = EnergyMax;
}

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = false;
	Type = EType::TypeUnit;
}

void AUnit::SetHP(uint8 a) { HP = a; }

uint8 AUnit::GetHP() { return HP; }

uint8 AUnit::GetHPMax() { return HPMax; }
