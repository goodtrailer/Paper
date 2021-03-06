// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "SkeletalUnit.h"
#include "Priest.generated.h"

/**
 * 
 */
UCLASS()
class PAPER_API APriest : public ASkeletalUnit
{
	GENERATED_BODY()
public:
	void DetermineAttackableTiles_Implementation(TSet<int>& OutReachableTiles, TSet<int>& OutAttackableTiles) const override;
};
