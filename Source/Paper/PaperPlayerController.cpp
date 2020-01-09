// Fill out your copyright notice in the Description page of Project Settings.

#include "PaperPlayerController.h"

APaperPlayerController::APaperPlayerController()
{
	bAutoManageActiveCameraTarget = false;
	bEnableClickEvents = true;
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Hand;
}

void APaperPlayerController::BeginPlay()
{
	SetViewTargetWithBlend(GetPawn(), 1.f);
	
}