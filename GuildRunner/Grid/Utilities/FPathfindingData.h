#pragma once
#include "CoreMinimal.h"
#include "FPathfindingData.generated.h"


USTRUCT(BlueprintType)
struct FPathfindingData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint Index = {-999, -999};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CostToEnterTile = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CostFromStart = 99999;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinimumCostToTarget = 99999;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint PreviousTile = {-999, -999};
};

