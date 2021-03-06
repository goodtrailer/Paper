// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "SkeletalUnit.h"
#include "Archer.generated.h"

/**
 * 
 */
UCLASS()
class PAPER_API AArcher : public ASkeletalUnit
{
	GENERATED_BODY()
public:
	void DetermineAttackableTiles_Implementation(TSet<int>& OutReachableTiles, TSet<int>& OutAttackableTiles) const override;
};
