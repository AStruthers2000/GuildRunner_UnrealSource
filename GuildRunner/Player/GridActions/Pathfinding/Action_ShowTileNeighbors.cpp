// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_ShowTileNeighbors.h"

#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Grid/CombatGridPathfinding.h"
#include "GuildRunner/Player/PlayerGridActions.h"

AAction_ShowTileNeighbors::AAction_ShowTileNeighbors()
{
}

void AAction_ShowTileNeighbors::ExecuteGridAction(FIntPoint TileIndex)
{
	Super::ExecuteGridAction(TileIndex);
	if(!PlayerGridActions) return;

	PlayerGridActions->GetCombatGridReference()->ClearStateFromTiles(IsNeighbor);
	auto Neighbors = PlayerGridActions->GetCombatGridReference()->GetGridPathfinding()->GetValidTileNeighbors(TileIndex, bIncludeDiagonals);
	for(auto& Neighbor : Neighbors)
	{
		PlayerGridActions->GetCombatGridReference()->AddStateToTile(Neighbor, IsNeighbor);
	}
}
