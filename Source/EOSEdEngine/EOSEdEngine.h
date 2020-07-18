// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "Editor/UnrealEdEngine.h"
#include "EOSEdEngine.generated.h"


UCLASS()
class EOSEDENGINE_API UEOSEdEngine : public UUnrealEdEngine
{
	GENERATED_BODY()
public:
	void Init(IEngineLoop* InEngineLoop) override;
	void PreExit() override;
};
