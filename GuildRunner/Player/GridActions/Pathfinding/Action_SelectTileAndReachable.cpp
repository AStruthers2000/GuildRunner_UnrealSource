// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_SelectTileAndReachable.h"

#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Grid/CombatGridPathfinding.h"
#include "GuildRunner/Grid/Utilities/ETileState.h"
#include "GuildRunner/Player/PlayerGridActions.h"

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

	//adds delegate to OnPathfindingComplete, which will change the color of each of the tiles to the IsReachable color
	PlayerGridActions->GetCombatGridReference()->GetGridPathfinding()->OnReachableTilesCompleted.AddDynamic(
		this, &AAction_SelectTileAndReachable::OnPathfindingCompleted);

	//call GetReachableTiles to kick off BFS floodfill algorithm
	PlayerGridActions->GetCombatGridReference()->GetGridPathfinding()->GetReachableTiles(
		PlayerGridActions->GetSelectedTile(), bIncludeDiagonals, GetValidWalkingTiles(), MaxPathLength);
}

/**
 * @brief Highlights each of the tiles within #MaxPathLength walking distance from the selected tile
 * 
 * @param Path Array of path indices representing the tiles that are in reachable range
 */
void AAction_SelectTileAndReachable::OnPathfindingCompleted(TArray<FIntPoint> Path)
{
	for (auto& TileInPath : Path)
	{
		PlayerGridActions->GetCombatGridReference()->AddStateToTile(TileInPath, IsReachable);
	}
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