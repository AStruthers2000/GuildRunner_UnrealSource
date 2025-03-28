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

	PlayerGridActions->OnSelectedTileChanged.AddDynamic(this, &AAction_FindPathToTarget::OnSelectedTileChanged);
}


void AAction_FindPathToTarget::ExecuteGridAction(FIntPoint TileIndex)
{
	Super::ExecuteGridAction(TileIndex);
	if (!PlayerGridActions)
	{
		return;
	}

	//PlayerGridActions->GetCombatGridReference()->DecrementTimesTileIsInPath(IsInPath);
	//PlayerGridActions->GetCombatGridReference()->ClearStateFromTiles(PathfindingTarget);

	PlayerGridActions->GetCombatGridReference()->GetGridPathfinding()->OnPathfindingCompleted.AddDynamic(
		this, &AAction_FindPathToTarget::OnPathfindingCompleted);

	PlayerGridActions->GetCombatGridReference()->GetGridPathfinding()->FindPath(
		PlayerGridActions->GetSelectedTile(), TileIndex, bIncludeDiagonals, false, GetValidWalkingTiles(), MaxPathLength);
}



void AAction_FindPathToTarget::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ExecuteGridAction(FPATHFINDINGDATA_DEFAULT_INDEX);
}

void AAction_FindPathToTarget::OnPathfindingCompleted(const TArray<FIntPoint>& Path)
{
	for (auto& TileInPath : Path)
	{
		PlayerGridActions->GetCombatGridReference()->IncrementTimesTileIsInPath(TileInPath);
	}

	if (Path.Num() > 0)
	{
		PlayerGridActions->GetCombatGridReference()->AddStateToTile(Path.Last(), PathfindingTarget);
	}
}

void AAction_FindPathToTarget::OnSelectedTileChanged(const FIntPoint& Index)
{
	//PlayerGridActions->GetCombatGridReference()->ClearStateFromTiles(IsInPath);
	//PlayerGridActions->GetCombatGridReference()->ClearStateFromTiles(PathfindingTarget);
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

