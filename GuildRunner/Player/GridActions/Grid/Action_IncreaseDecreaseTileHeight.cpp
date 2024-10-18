// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_IncreaseDecreaseTileHeight.h"

#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Player/PlayerGridActions.h"

AAction_IncreaseDecreaseTileHeight::AAction_IncreaseDecreaseTileHeight()
{
}

void AAction_IncreaseDecreaseTileHeight::ExecuteGridAction(FIntPoint TileIndex)
{
	Super::ExecuteGridAction(TileIndex);
	if (!PlayerGridActions)
	{
		return;
	}

	if (PlayerGridActions->GetCombatGridReference()->IsIndexValid(TileIndex))
	{
		auto Data = *PlayerGridActions->GetCombatGridReference()->GetGridTiles().Find(TileIndex);
		const auto LocationOffset = FVector(0, 0, PlayerGridActions->GetCombatGridReference()->GetGridTileSize().Z);
		const auto UpOrDown = bIncreaseHeight ? 1. : -1.0;
		Data.Transform.SetLocation(Data.Transform.GetLocation() + (LocationOffset * UpOrDown));
		PlayerGridActions->GetCombatGridReference()->AddGridTile(Data);
	}
}
