// Copyright (c) 2019–2020 Alden Wu

#pragma once

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
	void ZoomIn();
	void ZoomOut();
	void ResetPosition();

	UPROPERTY(BlueprintReadWrite)
		class USpringArmComponent* SpringArm;
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

	friend class APaperPlayerController;
};
