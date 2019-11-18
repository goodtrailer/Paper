// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PaperGameMode.h"
#include "PaperPlayerController.h"
#include "PaperPawn.h"

APaperGameMode::APaperGameMode()
{
	// no pawn by default
	DefaultPawnClass = APaperPawn::StaticClass();
	// use our own player controller class
	PlayerControllerClass = APaperPlayerController::StaticClass();
}
