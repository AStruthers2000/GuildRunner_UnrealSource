// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_MoveUnitOnGrid.h"

#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Grid/CombatGridPathfinding.h"
#include "GuildRunner/Player/PlayerGridActions.h"
#include "GuildRunner/Units/CombatGridUnit.h"
#include "GuildRunner/Units/CombatGridUnitMovement.h"

AAction_MoveUnitOnGrid::AAction_MoveUnitOnGrid()
{
}

void AAction_MoveUnitOnGrid::ExecuteGridAction(FIntPoint TileIndex)
{
	Super::ExecuteGridAction(TileIndex);
	if (!PlayerGridActions)
	{
		return;
	}

	CurrentUnit = Cast<ACombatGridUnit>(PlayerGridActions->GetSelectedGridObject());
	if (!CurrentUnit || CurrentUnit->GetCombatGridUnitMovement()->IsUnitCurrentlyMoving())
	{
		return;
	}

	//TODO: possibly refactor to instead have IsInPath be incremented and decremented for every unit moving 
	//PlayerGridActions->GetCombatGridReference()->ClearStateFromTiles(IsInPath);
	//PlayerGridActions->GetCombatGridReference()->ClearStateFromTiles(PathfindingTarget);

	PlayerGridActions->GetCombatGridReference()->GetGridPathfinding()->OnPathfindingCompleted.AddDynamic(
		this, &AAction_MoveUnitOnGrid::OnPathfindingCompleted);

	CurrentUnit->GetCombatGridUnitMovement()->OnCombatUnitFinishedWalking.AddDynamic(this, &AAction_MoveUnitOnGrid::OnUnitFinishedWalking);

	const auto bMoveDiagonal = CurrentUnit->GetUnitData().Stats.bCanMoveDiagonally;
	const auto ValidTileTypes = CurrentUnit->GetUnitData().Stats.ValidTileTypes;
	const auto MovementPoints = CurrentUnit->GetUnitData().Stats.MovementPoints;
	PlayerGridActions->GetCombatGridReference()->GetGridPathfinding()->FindPath(
		PlayerGridActions->GetSelectedTile(),
		TileIndex,
		bMoveDiagonal,
		false,
		ValidTileTypes,
		MovementPoints);
}


void AAction_MoveUnitOnGrid::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ExecuteGridAction(FPATHFINDINGDATA_DEFAULT_INDEX);
}

void AAction_MoveUnitOnGrid::OnPathfindingCompleted(const TArray<FIntPoint>& Path)
{
	if (!CurrentUnit || CurrentUnit != PlayerGridActions->GetSelectedGridObject())
	{
		return;
	}
	
	for (auto& TileInPath : Path)
	{
		PlayerGridActions->GetCombatGridReference()->IncrementTimesTileIsInPath(TileInPath);
	}
	
	if (Path.Num() > 0)
	{
		PlayerGridActions->GetCombatGridReference()->AddStateToTile(Path.Last(), PathfindingTarget);
	}
	
	CurrentUnit->GetCombatGridUnitMovement()->SetMoveDurationPerTile(MoveDurationPerTile);
	CurrentUnit->GetCombatGridUnitMovement()->UnitFollowPath(Path);
	PlayerGridActions->GetCombatGridReference()->ClearStateFromTiles(IsReachable);
	PlayerGridActions->DeselectCurrentTile();
	PlayerGridActions->DeselectCurrentObject();
}

void AAction_MoveUnitOnGrid::OnUnitFinishedWalking(ACombatGridUnit* Unit)
{
	//if (Unit == CurrentUnit)
	//{
		//PlayerGridActions->GetCombatGridReference()->ClearStateFromTiles(IsInPath);
		//PlayerGridActions->GetCombatGridReference()->ClearStateFromTiles(PathfindingTarget);
	//}

	// PlayerGridActions->GetCombatGridReference()->DecrementTimesTileIsInPath(Unit->GetIndexOnGrid());
	// PlayerGridActions->GetCombatGridReference()->RemoveStateFromTile(Unit->GetIndexOnGrid(), PathfindingTarget);
}
