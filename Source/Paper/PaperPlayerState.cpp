#include "PaperPlayerState.h"

#include "PaperGameState.h"
#include "PaperEnums.h"
#include "PaperGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

bool APaperPlayerState::IsTurn()
{
	return (GameState->Turn % GameState->BoardSpawns.Num() == static_cast<uint8>(Team)) ? true : false;
}

void APaperPlayerState::BeginPlay()
{
	GameState = GetWorld()->GetGameState<APaperGameState>();
	Team = GetGameInstance<UPaperGameInstance>()->Team;
}

void APaperPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APaperPlayerState, Team)
}

APaperPlayerState::APaperPlayerState()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}
