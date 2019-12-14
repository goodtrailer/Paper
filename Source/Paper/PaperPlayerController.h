// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "PaperPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PAPER_API APaperPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	APaperPlayerController();
	UPROPERTY(VisibleAnywhere)
		FString PawnName;
protected:
	void BeginPlay() override;
};
