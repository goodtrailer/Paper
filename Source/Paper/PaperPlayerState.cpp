#include "PaperPlayerState.h"

#include "PaperGameState.h"
#include "PaperEnums.h"
#include "PaperGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

bool APaperPlayerState::IsTurn()
{
	//GLog->Logf(TEXT("GameState: %s"), *FString(GameState ? "Valid" : "INVALID"));
	return (GameState->Turn % GameState->BoardSpawns.Num() == static_cast<uint8>(Team)) ? true : false;
}

void APaperPlayerState::BeginPlay()
{
	GameState = GetWorld()->GetGameState<APaperGameState>();
	GLog->Logf(TEXT("GameInstance Team: %d"), GetGameInstance<UPaperGameInstance>()->Team);
	Server_SetTeam(GetGameInstance<UPaperGameInstance>()->Team);
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

void APaperPlayerState::Server_SetTeam_Implementation(ETeam TeamToSet)
{
	GLog->Logf(TEXT("SetTeam Team: %d"), TeamToSet);
	Team = TeamToSet;
}

bool APaperPlayerState::Server_SetTeam_Validate(ETeam TeamToSet)
{
	return true;
}