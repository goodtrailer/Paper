// Copyright (c) 2019–2020 Alden Wu

#include "HotseatGameMode.h"
#include "CameraPawn.h"
#include "PaperEnums.h"
#include "Engine/World.h"
#include "PaperPlayerState.h"
#include "PaperGameState.h"
#include "PaperPlayerController.h"

void AHotseatGameMode::BeginGame()
{
	auto* PC = Cast<APaperPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	auto* PS = PC->GetPaperPlayerState();
	PS->SetTeam(ETeam::Green);

	APaperGameMode::BeginGame();
}

bool AHotseatGameMode::ParseBoardLayout(const uint8* BoardImage, const int BoardLayoutWidth, const int BoardLayoutHeight)
{
	bool bSuccess = APaperGameMode::ParseBoardLayout(BoardImage, BoardLayoutWidth, BoardLayoutHeight);
	for (auto& Status : GameState->TeamStatuses)
		Status = EStatus::Alive;
	return bSuccess;
}
