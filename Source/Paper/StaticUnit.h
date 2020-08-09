// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "StaticUnit.generated.h"

UCLASS()
class PAPER_API AStaticUnit : public AUnit
{
	GENERATED_BODY()
public:
	AStaticUnit();
	class UMeshComponent* GetMeshComponent() override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UStaticMeshComponent* StaticMeshComponent = nullptr;
};
