#include "Action_DeselectTile.h"

#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Grid/Utilities/FPathfindingData.h"
#include "GuildRunner/Player/PlayerGridActions.h"

AAction_DeselectTile::AAction_DeselectTile()
{
}

void AAction_DeselectTile::ExecuteGridAction(FIntPoint TileIndex)
{
	Super::ExecuteGridAction(TileIndex);
	if (!PlayerGridActions)
	{
		return;
	}

	PlayerGridActions->SelectTile(FPATHFINDINGDATA_DEFAULT_INDEX);
}

void AAction_DeselectTile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ExecuteGridAction(FPATHFINDINGDATA_DEFAULT_INDEX);
}
