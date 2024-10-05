#include "GridShapeUtilities.h"

#include "FGridShapeData.h"
#include "GuildRunner/Grid/Utilities/ETileType.h"

const FGridShapeData* UGridShapeUtilities::GetShapeData(EGridShape Shape)
{
	const auto* DataTable = FindGridDataMappingTable();
	
	if(!DataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("[UGuildRunnerUtilities::GetShapeData]:\tError: no grid data table assigned to this class, cannot spawn grid."));
		return nullptr;
	}

	if(Shape == NoDefinedShape)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UGuildRunnerUtilities::GetShapeData]:\tWarning: No defined shape selected, cannot spawn grid."));
		return nullptr;
	}
	
	const FName RowName = FName(UEnum::GetDisplayValueAsText(Shape).ToString());
	const FGridShapeData* OutRow = DataTable->FindRow<FGridShapeData>(RowName, "Tile Information", true);
	return OutRow;
}

bool UGridShapeUtilities::IsTileTypeWalkable(const ETileType TileType)
{
	const TArray NonWalkableTiles = {NoTile, Obstacle};
	return !NonWalkableTiles.Contains(TileType);
}

const UDataTable* UGridShapeUtilities::FindGridDataMappingTable()
{
	//if(GridDataMappingTable) return GridDataMappingTable;
	
	static ConstructorHelpers::FObjectFinder<UDataTable> GridShapeObject(TEXT("/Script/Engine.DataTable'/Game/GuildRunner/Grid/GridShapes/DT_GridShapeData.DT_GridShapeData'"));
	if (GridShapeObject.Succeeded())
	{
		//GridDataMappingTable = GridShapeObject.Object;
		//return GridDataMappingTable;
		return GridShapeObject.Object;
	}
	
	UE_LOG(LogTemp, Error, TEXT("[UGuildRunnerUtilities::FindGridDataMappingTable]: Could not find Grid Shape Data table."));
	return nullptr;
}
