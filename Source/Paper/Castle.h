// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "StaticUnit.h"
#include "Castle.generated.h"

UCLASS()
class PAPER_API ACastle : public AStaticUnit
{
	GENERATED_BODY()

public:
	ACastle();
	uint8 GetHP() const override;
	uint8 GetHPMax() const override;
	void SetHP(uint8 a) override;
	void DetermineMesh();
};
