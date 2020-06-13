#include "PaperPlayerState.h"

#include "PaperGameState.h"
#include "PaperEnums.h"
#include "PaperGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

bool APaperPlayerState::IsTurn()
{
	return (GetWorld()->GetGameState<APaperGameState>()->Turn % GetGameInstance<UPaperGameInstance>()->BoardInfo.SpawnNumber == static_cast<uint8>(GetGameInstance<UPaperGameInstance>()->Team)) ? true : false;
}

void APaperPlayerState::BeginPlay()
{
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
	Team = TeamToSet;
}

bool APaperPlayerState::Server_SetTeam_Validate(ETeam TeamToSet)
{
	return true;
}
