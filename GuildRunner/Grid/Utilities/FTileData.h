#pragma once
#include "CoreMinimal.h"
#include "ETileState.h"
#include "ETileType.h"
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
	TEnumAsByte<ETileType> Type = NoTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Transform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<ETileState>> States;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TimesSelected = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IndexOfSelectedObject = -1;

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
		IndexOfSelectedObject = -1;
	}

	void IncrementTimesSelected()
	{
		TimesSelected++;

		if (ObjectsOnTile.IsEmpty()) IndexOfSelectedObject = -1;
		else if (ObjectsOnTile.Num() == 1) IndexOfSelectedObject = 0;
		else IndexOfSelectedObject = (TimesSelected - 1) % ObjectsOnTile.Num();
	}

	ACombatGridObject* GetSelectedItem()
	{
		if (ObjectsOnTile.IsEmpty()) return nullptr;
		return ObjectsOnTile[IndexOfSelectedObject];
	}
};
