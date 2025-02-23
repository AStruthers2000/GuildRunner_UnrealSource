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

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//ACombatGridUnit* UnitOnTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ACombatGridObject*> ObjectOnTile;
};
