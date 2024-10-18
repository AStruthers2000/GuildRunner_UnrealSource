// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_RemoveTile.h"

#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Player/PlayerGridActions.h"

AAction_RemoveTile::AAction_RemoveTile()
{
}

void AAction_RemoveTile::ExecuteGridAction(FIntPoint TileIndex)
{
	Super::ExecuteGridAction(TileIndex);
	if (!PlayerGridActions)
	{
		return;
	}

	PlayerGridActions->GetCombatGridReference()->RemoveGridTile(TileIndex);
}
