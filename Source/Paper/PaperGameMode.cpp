// Fill out your copyright notice in the Description page of Project Settings.


#include "PaperGameMode.h"

APaperGameMode::APaperGameMode()
{
	PlayerControllerClass = APaperPlayerController::StaticClass();
	DefaultPawnClass = ACameraPawn::StaticClass();
	
}