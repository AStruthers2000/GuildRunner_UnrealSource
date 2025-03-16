// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Utilities/FPathfindingData.h"
#include "Utilities/FTileData.h"
#include "CombatGridPathfinding.generated.h"

struct FTileData;
class ACombatGrid;

constexpr int32 COMBAT_GRID_NORMAL_COST = 10;
constexpr int32 COMBAT_GRID_DIAG_COST = 4;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCombatPathfindingDataUpdateDelegate, const FIntPoint&, Index);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCombatPathfindingDestroyedDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCombatPathfindingCompletedDelegate, const TArray<FIntPoint>&, FullPath);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCombatReachableTilesCompletedDelegate, const TArray<FIntPoint>&, Reachables);

struct FTileNode
{
	FIntPoint Tile;
	int32 Cost;

	FTileNode()
		: Tile(FPATHFINDINGDATA_DEFAULT_INDEX), Cost(0) {}

	FTileNode(const FIntPoint& InTile, int32 InCost)
		: Tile(InTile), Cost(InCost) {}
};

UCLASS()
class GUILDRUNNER_API UCombatGridPathfinding : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	UCombatGridPathfinding();
	virtual void BeginPlay() override;

	void SetGridPathfindingReference(ACombatGrid* Reference) { GridReference = Reference; }

	//ONLY CALLED FROM DebugText
	TMap<FIntPoint, FPathfindingData> GetPathfindingData() const { return {}; }
	TArray<FIntPoint> GetDiscoveredTileIndices() const { return {}; }
	TArray<FIntPoint> GetAnalyzedTileIndices() const { return {}; }
	TArray<int32> GetDiscoveredTileSortingCosts() const { return {}; }

	/******************************************************************
	 * Pathfinding Callbacks
	 ******************************************************************/
	//NEVER CALLED, REMOVE LATER
	UPROPERTY(BlueprintAssignable, Category = "Test")
	FCombatPathfindingDataUpdateDelegate OnPathfindingDataUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Test")
	FCombatPathfindingDestroyedDelegate OnPathfindingDataCleared;

	UPROPERTY(BlueprintAssignable, Category = "Test")
	FCombatPathfindingCompletedDelegate OnPathfindingCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Test")
	FCombatReachableTilesCompletedDelegate OnReachableTilesCompleted;

private:
	UPROPERTY()
	ACombatGrid* GridReference = nullptr;

	/******************************************************************
	 * A* Pathfinding
	 ******************************************************************/
public:
	UFUNCTION(BlueprintCallable)
	TArray<FIntPoint> FindPath(FIntPoint StartTile, FIntPoint TargetTile, bool bUsingDiagonals, bool bCalculatingRange,
												   const TArray<TEnumAsByte<ETileType>>& ValidTileTypes, int32 Range);

	//UFUNCTION(BlueprintCallable)
	//void ClearGeneratedPathfindingData();

private:
	void ExecuteAsyncPathfinding(const FIntPoint& StartTile, const FIntPoint& TargetTile, bool bUsingDiagonals, const int32& Range) const;
	bool IsInputDataValid(bool bGeneratingReachables) const;
	int GetMinimumCostBetweenTwoTiles(const FIntPoint& Index1, const FIntPoint& Index2, bool bUsingDiagonals) const;

	FIntPoint StartIndex;
	FIntPoint TargetIndex;
	TArray<TEnumAsByte<ETileType>> ValidWalkableTiles = {Normal, DoubleCost, TripleCost, FlyingUnitsOnly};

	/******************************************************************
	 * Neighbor Checking
	 ******************************************************************/
public:
	UFUNCTION(BlueprintCallable)
	TArray<FPathfindingData> GetValidTileNeighbors(FIntPoint Index, bool bIncludeDiagonals) const;

private:
	TArray<FPathfindingData> GetNeighborIndicesForSquare(const FIntPoint& Index, bool bIncludeDiagonals) const;
	TArray<FPathfindingData> GetNeighborIndicesForHexagon(const FIntPoint& Index) const;
	TArray<FPathfindingData> GetNeighborIndicesForTriangle(const FIntPoint& Index, bool bIncludeDiagonals) const;
	TArray<FPathfindingData>
	CheckPotentialNeighbors(const FIntPoint& Index, TArray<FIntPoint> AttemptedNeighbors) const;
	bool ValidateNeighborIndex(const FTileData& InputTile, const FIntPoint& Neighbor,
	                           const TArray<TEnumAsByte<ETileType>>& ValidTileTypes) const;
	static int32 CalculateCostToEnterTile(const FTileData& InputTile);
	
};
