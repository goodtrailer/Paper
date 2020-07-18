// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "EOSGameEngine.generated.h"


UCLASS()
class PAPER_API UEOSGameEngine : public UGameEngine
{
	GENERATED_BODY()
public:
	void Init(IEngineLoop* InEngineLoop) override;
	void PreExit() override;
};
