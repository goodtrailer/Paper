// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUserInterface.generated.h"

enum class ETeam : uint8;

UCLASS()
class PAPER_API ULobbyUserInterface : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdatePlayerTeam(const FString& Name,  ETeam Team);
	UFUNCTION(BlueprintImplementableEvent)
	void RemoveLobbySlot(const FString& Name);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UBoardPreviewUserInterface* BoardPreviewInterface;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class USpinBox* TimerInterface;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class USpinBox* DelayCoefficientInterface;
};
