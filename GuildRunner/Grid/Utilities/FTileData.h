#pragma once
#include "CoreMinimal.h"
#include "ETileState.h"
#include "ETileType.h"
#include "GuildRunner/Grid/CombatGridObject.h"
#include "GuildRunner/Units/CombatGridUnit.h"
#include "FTileData.generated.h"


class ACombatGridObject;
class ACombatGridUnit;

USTRUCT(BlueprintType)
struct FTileData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint Index = {0, 0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETileType Type = ETileType::NoTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Transform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ETileState> States;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TimesSelected = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IndexOfSelectedObject = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TimesTileIsInPath = 0;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//ACombatGridUnit* UnitOnTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ACombatGridObject*> ObjectsOnTile;

	void TileSelected()
	{
		TimesSelected = 1;
		IndexOfSelectedObject = 0;
	}

	void TileDeselected()
	{
		TimesSelected = 0;
		IndexOfSelectedObject = 0;
	}

	void IncrementTimesSelected()
	{
		TimesSelected++;

		if (ObjectsOnTile.IsEmpty()) IndexOfSelectedObject = -1;
		else if (ObjectsOnTile.Num() == 1) IndexOfSelectedObject = 0;
		else IndexOfSelectedObject = (TimesSelected - 1) % ObjectsOnTile.Num();
	}

	ACombatGridObject* GetSelectedItem() const
	{
		if (ObjectsOnTile.IsEmpty()) return nullptr;
		return ObjectsOnTile[IndexOfSelectedObject];
	}

	void IncrementTimesInPath()
	{
		TimesTileIsInPath++;
	}

	void DecrementTimesInPath()
	{
		TimesTileIsInPath--;
		if (TimesTileIsInPath < 0) TimesTileIsInPath = 0;
	}

	void AddObjectToTile(ACombatGridObject* Object)
	{
		ObjectsOnTile.Add(Object);
		Object->SetIndexOnGrid(Index);
	}

	void RemoveObjectFromTile(ACombatGridObject* Object)
	{
		ObjectsOnTile.Remove(Object);
		Object->SetIndexOnGrid(FPATHFINDINGDATA_DEFAULT_INDEX);
	}
	
	ACombatGridUnit* TryGetUnitOnTile() const
	{
		if (ObjectsOnTile.IsEmpty()) return nullptr;
		
		for (const auto& Object : ObjectsOnTile)
		{
			auto* Unit = Cast<ACombatGridUnit>(Object);
			if (Unit->IsValidLowLevel()) return Unit;
		}

		return nullptr;
	}
};
