// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "CustomMeshComponent.h"
#include "TruncatedPrism.generated.h"

/**
* Why this class seems so over-engineered and why I won't be removing
* the now unused features:
* 
* Originally, HP bars weren't really an appealing option because I
* thought they would clutter the screen a lot (and they do). So I
* decided to make use of the team indicator circles below every unit.
* By truncating the circles (prisms), they could act as HP bars. This
* actually worked way better than I expected back when the only units
* implemented were the knight and miner, as both have models that leave
* the circle clearly visible. But this method ran into some issues with
* units like walls, castles, mines, and especially the new units like
* the scouts and catapults, where the circle was significantly harder
* to see. So now I've decided to resort to the HP bar idea (though I
* still dislike it). This code is still totally functional, and I may
* or may not revert to it or leave it as an option, I don't know, but
* it's code that I'm actually quite proud of.
* 
* TL;DR: it's code im proud of. it solved a problem well at the time,
* but it is no longer super good at solving that problem (not for any
* technical reasons).
* 
*/
UCLASS(meta = (BlueprintSpawnableComponent))
class PAPER_API UTruncatedPrism : public UCustomMeshComponent
{
	GENERATED_BODY()
public:
	UTruncatedPrism();
	void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void Truncate(float Fraction);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Properties")
	uint8 SideCount;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Properties")
	float Diameter;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Properties")
	float Thickness;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Properties")
	bool bIncludeBottomFace;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Properties")
	bool bTruncateZ;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties")
	float StartingFraction;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Properties")
	float CurrentFraction;
};
