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


USTRUCT(BlueprintType)
struct FPathfindingNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint Index = FPATHFINDINGDATA_DEFAULT_INDEX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int HCost;
	
	int FCost() const { return GCost + HCost; }

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSharedPtr<FPathfindingNode> Parent;

	FPathfindingNode() : GCost(0), HCost(0), Parent(nullptr) {}

	FPathfindingNode(const FIntPoint& InPosition, int32 InGCost, int32 InHCost, TSharedPtr<FPathfindingNode> InParent = nullptr)
		: Index(InPosition), GCost(InGCost), HCost(InHCost), Parent(InParent) {}
};