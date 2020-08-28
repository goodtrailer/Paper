// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "StaticUnit.h"
#include "Ground.generated.h"

enum class EDirection : uint8;

UCLASS()
class PAPER_API AGround : public AStaticUnit
{
	GENERATED_BODY()
public:
	void BuildMisc_Implementation() override;
	EDirection CollidableDirections;
};
