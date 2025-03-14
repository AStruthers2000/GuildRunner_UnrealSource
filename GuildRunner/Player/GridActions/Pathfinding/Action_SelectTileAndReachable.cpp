// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_SelectTileAndReachable.h"

#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Grid/CombatGridPathfinding.h"
#include "GuildRunner/Grid/Utilities/ETileState.h"
#include "GuildRunner/Player/PlayerGridActions.h"
#include "Kismet/GameplayStatics.h"
#include "Algo/RandomShuffle.h"

void AAction_SelectTileAndReachable::ExecuteGridAction(FIntPoint TileIndex)
{
	Super::ExecuteGridAction(TileIndex);

	GenerateReachables();
}

void AAction_SelectTileAndReachable::GenerateReachables()
{
	if (!PlayerGridActions)
	{
		return;
	}

	PlayerGridActions->GetCombatGridReference()->ClearStateFromTiles(IsReachable);

	PlayerGridActions->GetCombatGridReference()->GetGridPathfinding()->OnReachableTilesCompleted.AddDynamic(
		this, &AAction_SelectTileAndReachable::OnPathfindingCompleted);

	PlayerGridActions->GetCombatGridReference()->GetGridPathfinding()->FindPath(
		PlayerGridActions->GetSelectedTile(), FPATHFINDINGDATA_DEFAULT_INDEX, bIncludeDiagonals, true, GetValidWalkingTiles(), MaxPathLength);
}

/**
 * @brief Uses tick to add IsReachable state to one tile at a time (once each frame) instead of trying to add
 *	state to every tile at the same time. There is a hard crash (seemingly only with the diagonal movement enabled)
 *	that happens when "bulk" adding state to tile
 *	
 * @param DeltaSeconds Time between frames
 */
void AAction_SelectTileAndReachable::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (bAddingStateToTile)
	{
		const auto FrameStart = FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64());

		auto FrameNow = FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64()) - FrameStart;
		while (FrameNow < static_cast<double>(m_FrameDelayForDrawingReachables))
		{
			if (TilesToUpdateState.IsEmpty())
			{
				bAddingStateToTile = false;
			}
			else
			{
				const auto TileIndex = TilesToUpdateState.Pop();
				PlayerGridActions->GetCombatGridReference()->AddStateToTile(TileIndex, IsReachable);
			}
			FrameNow = FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64()) - FrameStart;
		}
	}
}

/**
 * @brief Highlights each of the tiles within #MaxPathLength walking distance from the selected tile
 * 
 * @param Path Array of path indices representing the tiles that are in reachable range
 */
void AAction_SelectTileAndReachable::OnPathfindingCompleted(const TArray<FIntPoint>& Path)
{
	TilesToUpdateState = Path;
	Algo::RandomShuffle(TilesToUpdateState);
	bAddingStateToTile = true;
	// for (auto& TileInPath : Path)
	// {
	// 	PlayerGridActions->GetCombatGridReference()->AddStateToTile(TileInPath, IsReachable);
	// }
}


TArray<TEnumAsByte<ETileType>> AAction_SelectTileAndReachable::GetValidWalkingTiles() const
{
	TArray<TEnumAsByte<ETileType>> ValidTiles = {Normal, DoubleCost, TripleCost};
	if (bFlyingUnit)
	{
		ValidTiles.Add(FlyingUnitsOnly);
	}

	return ValidTiles;
}