// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_SetTileType.h"

#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Player/PlayerGridActions.h"

AAction_SetTileType::AAction_SetTileType()
{
}

void AAction_SetTileType::ExecuteGridAction(FIntPoint TileIndex)
{
	Super::ExecuteGridAction(TileIndex);
	if(!PlayerGridActions) return;

	if(!PlayerGridActions) return;

	if(PlayerGridActions->GetCombatGridReference()->IsIndexValid(TileIndex))
	{
		auto Data = *PlayerGridActions->GetCombatGridReference()->GetGridTiles().Find(TileIndex);
		Data.Type = TileType;
		PlayerGridActions->GetCombatGridReference()->AddGridTile(Data);
	}
}
