// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_FindPathToTarget.h"

#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Grid/CombatGridPathfinding.h"
#include "GuildRunner/Player/PlayerGridActions.h"

AAction_FindPathToTarget::AAction_FindPathToTarget()
{
}

void AAction_FindPathToTarget::BeginPlay()
{
	Super::BeginPlay();

	//hook into OnReachableTilesCompleted so that we don't have to calculate them multiple times
	PlayerGridActions->GetCombatGridReference()->GetGridPathfinding()->OnReachableTilesCompleted.AddDynamic(
		this, &AAction_FindPathToTarget::OnReachableTilesCompleted);
}

void AAction_FindPathToTarget::ExecuteGridAction(FIntPoint TileIndex)
{
	Super::ExecuteGridAction(TileIndex);
	if (!PlayerGridActions)
	{
		return;
	}

	PlayerGridActions->GetCombatGridReference()->ClearStateFromTiles(IsInPath);

	//we first want to generate all reachable tiles for the selected unit
	//PlayerGridActions->GetCombatGridReference()->GetGridPathfinding()->OnReachableTilesCompleted.AddDynamic(
	//	this, &AAction_FindPathToTarget::OnReachableTilesCompleted);
	//PlayerGridActions->GetCombatGridReference()->GetGridPathfinding()->GetReachableTiles(
	//	PlayerGridActions->GetSelectedTile(), bIncludeDiagonals, GetValidWalkingTiles(), MaxPathLength);

	//Now that we've confirmed that we can reach the selected tile, we want to pathfind to that tile
	


	//const auto Reachables = PlayerGridActions->GetCombatGridReference()->GetGridPathfinding()->GetReachableTiles(
	//	PlayerGridActions->GetSelectedTile(), bIncludeDiagonals, GetValidWalkingTiles(), MaxPathLength);

	if (ReachableTiles.Contains(TileIndex))
	{
		PlayerGridActions->GetCombatGridReference()->GetGridPathfinding()->OnPathfindingCompleted.AddDynamic(
			this, &AAction_FindPathToTarget::OnPathfindingCompleted);
		
		PlayerGridActions->GetCombatGridReference()->GetGridPathfinding()->FindPath(
			PlayerGridActions->GetSelectedTile(), TileIndex, bIncludeDiagonals, GetValidWalkingTiles(),
			DelayBetweenIterations, MaxMsPerFrame);
	}
	else
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Selected tile is not reachable!!!!!!!");
	}
}



void AAction_FindPathToTarget::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ExecuteGridAction(FPATHFINDINGDATA_DEFAULT_INDEX);
}

void AAction_FindPathToTarget::OnReachableTilesCompleted(TArray<FIntPoint> Path)
{
	ReachableTiles = Path;
}

void AAction_FindPathToTarget::OnPathfindingCompleted(TArray<FIntPoint> Path)
{
	for (auto& TileInPath : Path)
	{
		PlayerGridActions->GetCombatGridReference()->AddStateToTile(TileInPath, IsInPath);
	}
}

TArray<TEnumAsByte<ETileType>> AAction_FindPathToTarget::GetValidWalkingTiles() const
{
	TArray<TEnumAsByte<ETileType>> ValidTiles = {Normal, DoubleCost, TripleCost};
	if (bFlyingUnit)
	{
		ValidTiles.Add(FlyingUnitsOnly);
	}

	return ValidTiles;
}
