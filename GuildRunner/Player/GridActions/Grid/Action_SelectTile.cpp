#include "Action_SelectTile.h"

#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Grid/Utilities/FPathfindingData.h"
#include "GuildRunner/Player/PlayerGridActions.h"

AAction_SelectTile::AAction_SelectTile()
{
}

void AAction_SelectTile::ExecuteGridAction(FIntPoint TileIndex)
{
	Super::ExecuteGridAction(TileIndex);
	if (!PlayerGridActions)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Executing Action_SelectTile trying to select tile %s"), *TileIndex.ToString());
	PlayerGridActions->SelectTile(TileIndex);
}

void AAction_SelectTile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ExecuteGridAction(FPATHFINDINGDATA_DEFAULT_INDEX);
}
