// Fill out your copyright notice in the Description page of Project Settings.


#include "PaperGameMode.h"
#include "CameraPawn.h"
#include "PaperPlayerController.h"

#include "EngineUtils.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"

APaperGameMode::APaperGameMode()
{
	PlayerControllerClass = APaperPlayerController::StaticClass();
	DefaultPawnClass = ACameraPawn::StaticClass();
	PrimaryActorTick.bCanEverTick = false;
	PlayerCount = 0;
}

void APaperGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void APaperGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	for (TActorIterator<ABoardGenerator> i(GetWorld()); i; ++i)
	{
		BoardGenerator = *i;
		BoardGenerator->SetOwner(NewPlayer);
	}

	NewPlayer->GetPlayerState<APlayerState>()->GetPawn<ACameraPawn>()->Client_SetTeam(static_cast<ETeam>(PlayerCount++));
	UE_LOG(LogTemp, Warning, TEXT("Number of players: %d"), GameState->PlayerArray.Num())
		UE_LOG(LogTemp, Warning, TEXT("Player Team: %d"), NewPlayer->GetPlayerState<APlayerState>()->GetPawn<ACameraPawn>()->Team)
		UE_LOG(LogTemp, Warning, TEXT("PlayerCount: %d"), PlayerCount)
}