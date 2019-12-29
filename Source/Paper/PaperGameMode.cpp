// Fill out your copyright notice in the Description page of Project Settings.


#include "PaperGameMode.h"
#include "CameraPawn.h"
#include "PaperPlayerController.h"

#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"

APaperGameMode::APaperGameMode()
{
	PlayerControllerClass = APaperPlayerController::StaticClass();
	DefaultPawnClass = ACameraPawn::StaticClass();
	PrimaryActorTick.bCanEverTick = true;
}

void APaperGameMode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay for APaperGameMode\nNumber of players: %d"), GameState->PlayerArray.Num())
	PlayerCount = 0;
}

void APaperGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (PlayerCount - GameState->PlayerArray.Num())
	{
		UE_LOG(LogTemp, Display, TEXT("setting teams"))
		uint8 temp = PlayerCount;
		for (int i = -1; i >= temp - GameState->PlayerArray.Num(); i--)
			GameState->PlayerArray[i + GameState->PlayerArray.Num()]->GetPawn<ACameraPawn>()->Client_SetTeam(PlayerCount++);
	}
}