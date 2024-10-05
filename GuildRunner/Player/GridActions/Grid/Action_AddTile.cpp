// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_AddTile.h"

#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Player/PlayerGridActions.h"

AAction_AddTile::AAction_AddTile()
{
}

void AAction_AddTile::ExecuteGridAction(FIntPoint TileIndex)
{
	Super::ExecuteGridAction(TileIndex);
	if(!PlayerGridActions) return;

	const auto Valid = PlayerGridActions->GetCombatGridReference()->IsIndexValid(TileIndex);
	if(!Valid)
	{
		FTransform NewTileTransform;
		NewTileTransform.SetLocation(PlayerGridActions->GetCombatGridReference()->GetTileLocationFromGridIndex(TileIndex));
		//FVector GroundLocation;
		//PlayerGridActions->GetCombatGridReference()->TraceForGround(NewTileTransform.GetLocation(), GroundLocation);
		//NewTileTransform.SetLocation(GroundLocation);
		NewTileTransform.SetRotation(PlayerGridActions->GetCombatGridReference()->GetTileRotationFromGridIndex(TileIndex).Quaternion());
		NewTileTransform.SetScale3D(PlayerGridActions->GetCombatGridReference()->GetTileScale());
		
		FTileData NewTileData;
		NewTileData.Index = TileIndex;
		NewTileData.Type = Normal;
		NewTileData.Transform = NewTileTransform;
		
		PlayerGridActions->GetCombatGridReference()->AddGridTile(NewTileData);
	}
}
