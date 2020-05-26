// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PaperGameInstance.generated.h"

enum class ETeam : uint8;

struct FBoardInfo
{
	int SizeX, SizeY;
	uint8 SpawnNumber;
};

UCLASS()
class PAPER_API UPaperGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	FBoardInfo BoardInfo = {20, 28, 2};
	ETeam Team;
};
