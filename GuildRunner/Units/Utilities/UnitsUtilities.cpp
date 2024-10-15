// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitsUtilities.h"

#include "FCombatGridUnitData.h"
#include "Engine/DataTable.h"

const FCombatGridUnitData* UUnitsUtilities::GetDefaultUnitDataFromUnitType(EUnitType UnitType)
{
	const auto* DataTable = FindGridDataMappingTable();
	
	if(!DataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("[UUnitsUtilities::GetDefaultUnitDataFromUnitType]:\tError: no unit data table assigned to this class, cannot spawn grid."));
		return nullptr;
	}

	//if(UnitType == NoDefinedShape)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("[UGuildRunnerUtilities::GetShapeData]:\tWarning: No defined shape selected, cannot spawn grid."));
	//	return nullptr;
	//}
	
	const FName RowName = FName(UEnum::GetDisplayValueAsText(UnitType).ToString());
	const FCombatGridUnitData* OutRow = DataTable->FindRow<FCombatGridUnitData>(RowName, "Unit Information", true);
	if(OutRow) return OutRow;
	UE_LOG(LogTemp, Error, TEXT("[UUnitsUtilities::GetDefaultUnitDataFromUnitType]:\t Could not dereference row information"));
	return nullptr;
}

FCombatGridUnitData UUnitsUtilities::GetDefaultUnitDataFromUnitType_BP(TEnumAsByte<EUnitType> UnitType)
{
	const auto* Data = GetDefaultUnitDataFromUnitType(UnitType);
	if(Data)
	{
		return *Data;
	}
	
	return {};
}

const UDataTable* UUnitsUtilities::FindGridDataMappingTable()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> GridShapeObject(TEXT("/Script/Engine.DataTable'/Game/GuildRunner/Units/Utilities/DT_DefaultUnitDataPerType.DT_DefaultUnitDataPerType'"));
	if (GridShapeObject.Succeeded())
	{
		//GridDataMappingTable = GridShapeObject.Object;
		//return GridDataMappingTable;
		return GridShapeObject.Object;
	}
	
	UE_LOG(LogTemp, Error, TEXT("[UUnitsUtilities::FindGridDataMappingTable]: Could not find Grid Shape Data table."));
	return nullptr;
}
