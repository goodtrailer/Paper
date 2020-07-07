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
	UFUNCTION()
	void OnRep_Team();
	UFUNCTION()
	void OnRep_Name();
	void SetTeam(ETeam NewTeam);				// This is a local function that should be called by the server. This only exists for OnReps. Calling on client will do nothing.
	void SetName(const FString& NewName);		// This is a local function that should be called by the server. This only exists for OnReps. Calling on client will do nothing.
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;

	UPROPERTY(ReplicatedUsing = OnRep_Team, VisibleAnywhere, BlueprintReadWrite, Category = "Meta")
	ETeam Team;									// If you're setting this variable, you might want to call SetTeam instead because it includes an OnRep call. OnRep doesn't occur server-side. Functionally it will be the same, but the host's GUI may display some incorrent information.
	UPROPERTY(ReplicatedUsing = OnRep_Name, VisibleAnywhere, BlueprintReadWrite, Category = "Meta")
	FString Name;								// If you're setting this variable, you might want to call SetName instead because it includes an OnRep call. OnRep doesn't occur server-side. Functionally it will be the same, but the host's GUI may display some incorrent information.
};
