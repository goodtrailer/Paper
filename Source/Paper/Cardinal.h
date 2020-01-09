// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Cardinal.generated.h"

USTRUCT(BlueprintType)
struct FCardinal
{
	GENERATED_BODY()

	FCardinal();

	enum EDirection: unsigned char
	{
		Up = 0, Right, Down, Left
	};

	
	void Set(unsigned char d, bool b);
	bool Get(unsigned char d);

	UPROPERTY(BlueprintReadWrite)
	TArray<bool> Booleans;
};
