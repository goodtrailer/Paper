// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "CustomMeshComponent.h"
#include "TruncatedPrism.generated.h"

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
