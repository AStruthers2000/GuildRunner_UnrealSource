// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_FindPathToTarget.h"

#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Grid/CombatGridPathfinding.h"
#include "GuildRunner/Player/PlayerGridActions.h"

AAction_FindPathToTarget::AAction_FindPathToTarget()
{
}

void AAction_FindPathToTarget::ExecuteGridAction(FIntPoint TileIndex)
{
	Super::ExecuteGridAction(TileIndex);
	if(!PlayerGridActions) return;

	PlayerGridActions->GetCombatGridReference()->ClearStateFromTiles(IsInPath);
	auto Path = PlayerGridActions->GetCombatGridReference()->GetGridPathfinding()->FindPath(PlayerGridActions->GetSelectedTile(), TileIndex, bIncludeDiagonals);
	for(auto& TileInPath : Path)
	{
		PlayerGridActions->GetCombatGridReference()->AddStateToTile(TileInPath, IsInPath);
	}
}
