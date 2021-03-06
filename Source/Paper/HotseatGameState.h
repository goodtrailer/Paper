// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "PaperGameState.h"
#include "HotseatGameState.generated.h"

UCLASS()
class PAPER_API AHotseatGameState : public APaperGameState
{
	GENERATED_BODY()
public:
	virtual void Defeat(ETeam DefeatedTeam) override;
	virtual void EndTurn() override;
	virtual void BeginPlay() override;

private:
	APawn* Pawn;
};
