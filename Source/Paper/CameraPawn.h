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
	UFUNCTION(Client, Reliable)
	void Client_SetTeam(ETeam t);
	UFUNCTION(Server, Reliable, BlueprintCallable, WithValidation)
	void Server_SpawnUnit(ETeam team, TSubclassOf<AUnit> type);
	UFUNCTION(Server, Reliable, BlueprintCallable, WithValidation)
	void Server_EndTurn();
	UFUNCTION(BlueprintCallable)
	bool IsTurn();

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
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EndTurn();
	void EndTurn();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnUnit(ETeam team, TSubclassOf<AUnit> type);
	void SpawnUnit(ETeam team, TSubclassOf<AUnit> type);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Debug();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Debug();
	void Debug();

	void MoveUnit();
	void SelectUnit();
	void MoveOverlayOn();
	void MoveOverlayOff();

public:
	UPROPERTY(BlueprintReadOnly)
	ABoardGenerator* BoardGenerator;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	ETeam Team;
protected:
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	bool bPanButtonDown;
	bool bRotateButtonDown;
	bool bSelectButtonDown;
	bool bMoveButtonDown;

	float MouseX;
	float MouseY;
	AUnit* SelectedUnit;
	AUnit* HoveredUnit;
	APlayerController* PlayerController;

	UPROPERTY(EditAnywhere, Category = "Sensitivity")
	float RotateSensitivity;
	UPROPERTY(EditAnywhere, Category = "Sensitivity")
	float ZoomSensitivity;
	UPROPERTY(EditAnywhere, Category = "Sensitivity")
	float PanSensitivity;
};
