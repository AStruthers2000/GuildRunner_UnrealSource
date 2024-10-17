// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_MoveUnitOnGrid.h"

#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Grid/CombatGridPathfinding.h"
#include "GuildRunner/Player/PlayerGridActions.h"
#include "GuildRunner/Units/CombatGridUnit.h"

AAction_MoveUnitOnGrid::AAction_MoveUnitOnGrid()
{
}

void AAction_MoveUnitOnGrid::ExecuteGridAction(FIntPoint TileIndex)
{
	Super::ExecuteGridAction(TileIndex);
	if(!PlayerGridActions) return;

	CurrentUnit = PlayerGridActions->GetSelectedUnit();
	if(!CurrentUnit) return;

	CurrentUnit->OnCombatUnitFinishedWalking.AddDynamic(this, &AAction_MoveUnitOnGrid::OnUnitFinishedWalking);
	PlayerGridActions->GetCombatGridReference()->ClearStateFromTiles(IsInPath);
	PlayerGridActions->GetCombatGridReference()->GetGridPathfinding()->OnPathfindingCompleted.AddDynamic(this, &AAction_MoveUnitOnGrid::OnPathfindingCompleted);	
	
	const auto bMoveDiagonal = CurrentUnit->GetUnitData().Stats.bCanMoveDiagonally;
	const auto ValidTileTypes = CurrentUnit->GetUnitData().Stats.ValidTileTypes;
	PlayerGridActions->GetCombatGridReference()->GetGridPathfinding()->FindPath(
		PlayerGridActions->GetSelectedTile(),
		TileIndex,
		bMoveDiagonal,
		ValidTileTypes,
		0.f,
		0.f);
}


void AAction_MoveUnitOnGrid::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ExecuteGridAction(FPATHFINDINGDATA_DEFAULT_INDEX);
}

void AAction_MoveUnitOnGrid::OnPathfindingCompleted(TArray<FIntPoint> Path)
{
	if(!CurrentUnit) return;
	if(CurrentUnit != PlayerGridActions->GetSelectedUnit()) return;
	
	for(auto& TileInPath : Path)
	{
		PlayerGridActions->GetCombatGridReference()->AddStateToTile(TileInPath, IsInPath);
	}
	CurrentUnit->SetMoveDurationPerTile(MoveDurationPerTile);
	CurrentUnit->UnitFollowPath(Path);
}

void AAction_MoveUnitOnGrid::OnUnitFinishedWalking(ACombatGridUnit* Unit)
{
	if(Unit == CurrentUnit)
	{
		PlayerGridActions->GetCombatGridReference()->ClearStateFromTiles(IsInPath);
	}
}
