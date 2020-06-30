// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PaperPlayerState.generated.h"


enum class ETeam : uint8;

UCLASS()
class PAPER_API APaperPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	APaperPlayerState();
	UFUNCTION(BlueprintCallable)
	bool IsTurn();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_SetTeam(ETeam TeamToSet);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Meta")
	ETeam Team;
};
