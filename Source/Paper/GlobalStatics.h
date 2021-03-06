// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GlobalStatics.generated.h"

UCLASS(Blueprintable)
class PAPER_API UGlobalStatics : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Statics")
	TArray<class UMaterialInterface*> HPPrismMeterMaterials;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Statics")
	class UStaticMesh* GroundArrowMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Statics")
	class USoundWave* DieSound;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Statics")
	class USoundWave* EndTurnSound;
};
