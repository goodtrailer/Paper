// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Cardinal.h"
#include "Unit.h"
#include "GameFramework/SpringArmComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CameraPawn.generated.h"


UCLASS()
class PAPER_API ACameraPawn : public APawn
{
	GENERATED_BODY()
public:
	ACameraPawn();
	virtual void Tick(float) override;
	void BeginPlay() override;

protected:
	//void SpawnUnit(ETeam, TSubclassOf<AUnit>);
	void ZoomIn();
	void ZoomOut();

	UPROPERTY(BlueprintReadWrite)
		USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, Category = "Sensitivity")
		float RotateSensitivity;
	UPROPERTY(EditAnywhere, Category = "Sensitivity")
		float ZoomSensitivity;
	UPROPERTY(EditAnywhere, Category = "Sensitivity")
		float PanSensitivity;
	UPROPERTY(VisibleAnywhere, Category = "Controls State")
		bool bIsPanning;
	UPROPERTY(VisibleAnywhere, Category = "Controls State")
		bool bIsRotating;
	UPROPERTY(VisibleAnywhere, Category = "Controls State")
		float MouseDeltaX;
	UPROPERTY(VisibleAnywhere, Category = "Controls State")
		float MouseDeltaY;
	//APaperPlayerState* PlayerState;
	//APaperPlayerController* PlayerController;

	friend class APaperPlayerController;
};
