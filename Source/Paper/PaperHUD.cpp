// Copyright (c) 2019–2020 Alden Wu


#include "PaperHUD.h"
#include "PaperGameState.h"
#include "Unit.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Canvas.h"

#define MAX_HP_BAR_COLOR FLinearColor::Green
#define MIN_HP_BAR_COLOR FLinearColor::Red
#define EMPTY_HP_BAR_COLOR FLinearColor(0.8f, 0.8f, 0.8f)

#define CURSOR_RADIUS_THRESHOLD_CIRCLE_COLOR FLinearColor::Yellow

void APaperHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D MousePos;

	if (PlayerController)
		PlayerController->GetMousePosition(MousePos.X, MousePos.Y);

	if (bShowHPBars && GameState)
	{
		int BoardWidth = GameState->GetBoardWidth();

		{
			FVector2D FullBarScreenPos;
			for (auto& Unit : GameState->UnitBoard)
				if (Unit)
				{
					FVector WorldPos = Unit->GetActorLocation();
					WorldPos.Z += 200;									// hp bar floats above unit
					UGameplayStatics::ProjectWorldToScreen(PlayerController, WorldPos, FullBarScreenPos);

					if (FVector2D::Distance(FullBarScreenPos, MousePos) > CursorRadiusThreshold)
						continue;

					FullBarScreenPos.X -= HPBarLength / 2;
					FullBarScreenPos.Y -= HPBarHeight / 2;

					const float PercentHP = (float)Unit->GetHP() / Unit->GetHPMax();
					const int FilledBarLength = PercentHP * HPBarLength;

					// draw filled bar using smoothstep_2 equation
					const float SmoothInterp = 6 * FMath::Pow(PercentHP, 5) - 15 * FMath::Pow(PercentHP, 4) + 10 * FMath::Pow(PercentHP, 3);
					DrawRect(FLinearColor::LerpUsingHSV(MIN_HP_BAR_COLOR, MAX_HP_BAR_COLOR, SmoothInterp),
						FullBarScreenPos.X, FullBarScreenPos.Y,
						FilledBarLength, HPBarHeight);
					// draw empty bar
					DrawRect(EMPTY_HP_BAR_COLOR,
						FullBarScreenPos.X + FilledBarLength, FullBarScreenPos.Y,
						HPBarLength * (1 - PercentHP), HPBarHeight);
				}
		}
	}

	if (bShowRadiusThresholdCircle)
	{
		constexpr int N = 24;
		constexpr float AngleIncrement = 2 * PI / N;
		FVector2D StartPoint;
		FVector2D EndPoint;

		StartPoint.X = MousePos.X + CursorRadiusThreshold;
		StartPoint.Y = MousePos.Y;

		for (uint8 i = 0; i < N; i++)
		{
			EndPoint.X = CursorRadiusThreshold * FMath::Cos((i + 1) * AngleIncrement) + MousePos.X;
			EndPoint.Y = CursorRadiusThreshold * FMath::Sin((i + 1) * AngleIncrement) + MousePos.Y;
			DrawLine(StartPoint.X, StartPoint.Y, EndPoint.X, EndPoint.Y, CURSOR_RADIUS_THRESHOLD_CIRCLE_COLOR, 2.f);
			StartPoint = EndPoint;
		}
	}
}

void APaperHUD::BeginPlay()
{
	Super::BeginPlay();

	GameState = GetWorld()->GetGameState<APaperGameState>();
	PlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
}
