// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BoardGenerator.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CameraPawn.generated.h"

UCLASS()
class PAPER_API ACameraPawn : public APawn
{
	GENERATED_BODY()

public:
	ACameraPawn();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
	void SetTruebPanButtonDown();
	void SetFalsebPanButtonDown();
	void SetTruebRotateButtonDown();
	void SetFalsebRotateButtonDown();
	void SetMouseX(float f);
	void SetMouseY(float f);
	void ZoomIn();
	void ZoomOut();
	void Debug();
	/*
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerDebug();
	*/

public:
	ABoardGenerator* BoardGenerator;
protected:
	UPROPERTY(VisibleAnywhere)
		class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere)
		class USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere)
		class USpringArmComponent* SpringArm;

	bool bPanButtonDown;
	bool bRotateButtonDown;
	float MouseX;
	float MouseY;

	UPROPERTY(EditAnywhere, Category = "Sensitivity")
		float RotateSensitivity;
	UPROPERTY(EditAnywhere, Category = "Sensitivity")
		float ZoomSensitivity;
	UPROPERTY(EditAnywhere, Category = "Sensitivity")
		float PanSensitivity;

};
