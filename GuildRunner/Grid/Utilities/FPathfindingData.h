#pragma once
#include "CoreMinimal.h"
#include "FPathfindingData.generated.h"

#define FPATHFINDINGDATA_DEFAULT_INDEX {-999, -999}
#define FPATHFINDINGDATA_DEFAULT_ROUTING_COST 99999
#define FPATHFINDINGDATA_DEFAULT_TILE_COST 1

USTRUCT(BlueprintType)
struct FPathfindingData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint Index = FPATHFINDINGDATA_DEFAULT_INDEX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CostToEnterTile = FPATHFINDINGDATA_DEFAULT_TILE_COST;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CostFromStart = FPATHFINDINGDATA_DEFAULT_ROUTING_COST;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinimumCostToTarget = FPATHFINDINGDATA_DEFAULT_ROUTING_COST;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint PreviousTile = FPATHFINDINGDATA_DEFAULT_INDEX;
};

