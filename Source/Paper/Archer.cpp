// Copyright (c) 2019–2020 Alden Wu

#include "Archer.h"
#include "PaperGameState.h"
#include "GenericPlatform/GenericPlatformMath.h"

inline bool BresenhamGentle(int x0, int y0, int x1, int y1, int BoardWidth, ETeam SourceTeam, const TArray<AUnit*>& UnitBoard)
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	int i = 1;
	if (dy < 0)
	{
		i *= -1;
		dy *= -1;
	}

	// Bresenham calculations (error accumulation)
	int Error = 2 * dy - dx;
	int y = y0;
	for (int x = x0; x <= x1; x++)
	{
		AUnit* BlockingUnit = UnitBoard[x + y * BoardWidth];
		// if greatshield is on the line of sight, there is no clear line of sight
		if (BlockingUnit && BlockingUnit->Type == EType::Greatshield && BlockingUnit->Team != SourceTeam)
			return false;
		if (Error > 0)
		{
			y += i;
			Error -= 2 * dx;
		}
		Error += 2 * dy;
	}
	// line of sight is clear
	return true;
}

// same as BresenhamGentle but flipped to iterate through y values instead of x values
inline bool BresenhamSteep(int x0, int y0, int x1, int y1, int BoardWidth, ETeam SourceTeam, const TArray<AUnit*>& UnitBoard)
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	int i = 1;
	if (dx < 0)
	{
		i *= -1;
		dx *= -1;
	}

	int Error = 2 * dx - dy;
	int x = x0;
	for (int y = y0; y <= y1; y++)
	{
		AUnit* BlockingUnit = UnitBoard[x + y * BoardWidth];
		if (BlockingUnit && BlockingUnit->Type == EType::Greatshield && BlockingUnit->Team != SourceTeam)
			return false;
		if (Error > 0)
		{
			x += i;
			Error -= 2 * dy;
		}
		Error += 2 * dx;
	}
	return true;
}

inline bool CheckClearLineOfSight(int SourceCoordinates, int TargetCoordinates, int BoardWidth, ETeam SourceTeam, const TArray<AUnit*>& UnitBoard) 
{
	// Bresenham's for all octants (thanks wikipedia, pretty cool explanations)
	int x0 = SourceCoordinates % BoardWidth;
	int y0 = SourceCoordinates / BoardWidth;
	int x1 = TargetCoordinates % BoardWidth;
	int y1 = TargetCoordinates / BoardWidth;

	if (FGenericPlatformMath::Abs(y1 - y0) < FGenericPlatformMath::Abs(x1 - x0))
		if (x0 < x1)
			return BresenhamGentle(x0, y0, x1, y1, BoardWidth, SourceTeam, UnitBoard);
		else
			return BresenhamGentle(x1, y1, x0, y0, BoardWidth, SourceTeam, UnitBoard);
	else
		if (y0 < y1)
			return BresenhamSteep(x0, y0, x1, y1, BoardWidth, SourceTeam, UnitBoard);
		else
			return BresenhamSteep(x1, y1, x0, y0, BoardWidth, SourceTeam, UnitBoard);
}

void AArcher::DetermineAttackableTiles_Implementation(TSet<int>& OutReachableTiles, TSet<int>& OutAttackableTiles) const
{
	APaperGameState* GameState = GetWorld()->GetGameState<APaperGameState>();
	const int BoardWidth = GameState->GetBoardWidth();
	const int BoardHeight = GameState->GetBoardHeight();

	for (int i = 0; i <= Range; i++)
		for (int j = i - Range; j <= Range - i; j++)
			if (Coordinates / BoardWidth + j < BoardHeight && Coordinates / BoardWidth + j >= 0)
			{
				if (Coordinates % BoardWidth + i < BoardWidth)
				{
					int ReachableCoord = Coordinates + i + j * BoardWidth;
					if (CheckClearLineOfSight(Coordinates, ReachableCoord, BoardWidth, Team, GameState->UnitBoard))
					{
						OutReachableTiles.Add(ReachableCoord);
						AUnit* ReachableUnit = GameState->UnitBoard[ReachableCoord];
						if (ReachableUnit && ReachableUnit->Team != Team && ReachableUnit->bIsTargetable)
							OutAttackableTiles.Add(ReachableCoord);
					}
				}
				if (Coordinates % BoardWidth - i >= 0)
				{
					int ReachableCoord = Coordinates - i + j * BoardWidth;
					if (CheckClearLineOfSight(Coordinates, ReachableCoord, BoardWidth, Team, GameState->UnitBoard))
					{
						OutReachableTiles.Add(ReachableCoord);
						AUnit* ReachableUnit = GameState->UnitBoard[ReachableCoord];
						if (ReachableUnit && ReachableUnit->Team != Team && ReachableUnit->bIsTargetable)
							OutAttackableTiles.Add(ReachableCoord);
					}
				}
			}
}
