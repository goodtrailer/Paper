// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "PaperGameMode.h"
#include "HotseatGameMode.generated.h"


UCLASS()
class PAPER_API AHotseatGameMode : public APaperGameMode
{
	GENERATED_BODY()
public:
	void BeginGame() override;
private:
	bool ParseBoardLayout(const uint8* BoardImage, const int BoardLayoutWidth, const int BoardLayoutHeight) override;
};
