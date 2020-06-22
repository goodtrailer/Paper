// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "Castle.generated.h"

UCLASS()
class PAPER_API ACastle : public AUnit
{
	GENERATED_BODY()
	
public:
	uint8 GetHP() const override;
	uint8 GetHPMax() const override;
	void SetHP(uint8 a) override;
};
