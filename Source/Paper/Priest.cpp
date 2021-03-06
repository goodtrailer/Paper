// Copyright (c) 2019–2020 Alden Wu

#include "Priest.h"
#include "PaperGameState.h"

void APriest::DetermineAttackableTiles_Implementation(TSet<int>& OutReachableTiles, TSet<int>& OutAttackableTiles) const
{
	APaperGameState* GameState = GetWorld()->GetGameState<APaperGameState>();
	const int BoardWidth = GameState->GetBoardWidth();
	const int BoardHeight = GameState->GetBoardHeight();

	for (int i = 0; i <= Range; i++)
		for (int j = -Range; j <= Range; j++)
			if (Coordinates / BoardWidth + j < BoardHeight && Coordinates / BoardWidth + j >= 0)
			{
				if (Coordinates % BoardWidth + i < BoardWidth)
				{
					int ReachableCoord = Coordinates + i + j * BoardWidth;
					AUnit* ReachableUnit = GameState->UnitBoard[ReachableCoord];
					OutReachableTiles.Add(ReachableCoord);
					if (ReachableUnit && ReachableUnit->Team == Team && ReachableUnit->bIsTargetable)
						OutAttackableTiles.Add(ReachableCoord);
				}
				if (Coordinates % BoardWidth - i >= 0)
				{
					int ReachableCoord = Coordinates - i + j * BoardWidth;
					AUnit* ReachableUnit = GameState->UnitBoard[ReachableCoord];
					OutReachableTiles.Add(ReachableCoord);
					if (ReachableUnit && ReachableUnit->Team == Team && ReachableUnit->bIsTargetable)
						OutAttackableTiles.Add(ReachableCoord);
				}
			}
}