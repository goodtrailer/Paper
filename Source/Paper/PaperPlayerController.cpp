// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PaperPlayerController.h"

APaperPlayerController::APaperPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}
