// Copyright (c) 2019–2020 Alden Wu

#include "Archer.h"
#include "PaperGameState.h"
#include "GenericPlatform/GenericPlatformMath.h"

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
					TArray<FHitResult> Hits;
					GetWorld()->LineTraceMultiByChannel(Hits, GetActorLocation(), FVector(ReachableCoord % BoardWidth * 200, ReachableCoord / BoardWidth * 200, 200), ECC_GameTraceChannel2);

					bool bIsBlocked = false;
					for (auto& Hit : Hits)
					{
						if (AUnit* HitUnit = Cast<AUnit>(Hit.GetActor()))
							if (HitUnit->Team != Team && HitUnit->Coordinates != ReachableCoord)
							{
								bIsBlocked = true;
								break;
							}
					}
					if (!bIsBlocked)
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
					TArray<FHitResult> Hits;
					GetWorld()->LineTraceMultiByChannel(Hits, GetActorLocation(), FVector(ReachableCoord % BoardWidth * 200, ReachableCoord / BoardWidth * 200, 200), ECC_GameTraceChannel2);
					
					bool bIsBlocked = false;
					for (auto& Hit : Hits)
					{
						if (AUnit* HitUnit = Cast<AUnit>(Hit.GetActor()))
							if (HitUnit->Team != Team && HitUnit->Coordinates != ReachableCoord)
							{
								bIsBlocked = true;
								break;
							}
					}
					if (!bIsBlocked)
					{
						OutReachableTiles.Add(ReachableCoord);
						AUnit* ReachableUnit = GameState->UnitBoard[ReachableCoord];
						if (ReachableUnit && ReachableUnit->Team != Team && ReachableUnit->bIsTargetable)
							OutAttackableTiles.Add(ReachableCoord);
					}
				}
			}
}
