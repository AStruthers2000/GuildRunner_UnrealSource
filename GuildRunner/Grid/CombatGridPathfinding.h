// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Utilities/FPathfindingData.h"
#include "CombatGridPathfinding.generated.h"

struct FTileData;
class ACombatGrid;

#define COMBAT_GRID_NORMAL_COST 10
#define COMBAT_GRID_DIAG_COST 14

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCombatPathfindingDataUpdateDelegate, FIntPoint, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCombatPathfindingDestroyedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCombatPathfindingCompletedDelegate, TArray<FIntPoint>, FullPath);

UCLASS()
class GUILDRUNNER_API UCombatGridPathfinding : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UCombatGridPathfinding();
	virtual void BeginPlay() override;

	void SetGridPathfindingReference(ACombatGrid* Reference) { GridReference = Reference; }
	TMap<FIntPoint, FPathfindingData> GetPathfindingData() const { return PathfindingData; }
	TArray<FIntPoint> GetDiscoveredTileIndices() const { return DiscoveredTileIndices; }
	TArray<FIntPoint> GetAnalyzedTileIndices() const { return AnalyzedTileIndices; }
	TArray<int32> GetDiscoveredTileSortingCosts() const { return DiscoveredTileSortingCosts; }

	/******************************************************************
	 * Pathfinding Callbacks
	 ******************************************************************/
	UPROPERTY(BlueprintAssignable, Category = "Test")
	FCombatPathfindingDataUpdateDelegate OnPathfindingDataUpdated;
	
	UPROPERTY(BlueprintAssignable, Category = "Test")
	FCombatPathfindingDestroyedDelegate OnPathfindingDataCleared;

	UPROPERTY(BlueprintAssignable, Category = "Test")
	FCombatPathfindingCompletedDelegate OnPathfindingCompleted;

private:
	UPROPERTY()
	ACombatGrid* GridReference = nullptr;

	/******************************************************************
	 * A* Pathfinding
	 ******************************************************************/
public:
	UFUNCTION(BlueprintCallable)
	TArray<FIntPoint> FindPath(FIntPoint StartTile, FIntPoint TargetTile, bool bUsingDiagonals, float Delay, float MaxMs);

	UFUNCTION(BlueprintCallable)
	void ClearGeneratedPathfindingData();

private:
	bool IsInputDataValid() const;
	void DiscoverTile(const FPathfindingData& TilePathData);
	int GetMinimumCostBetweenTwoTiles(const FIntPoint& Index1, const FIntPoint& Index2, bool bUsingDiagonals) const;
	bool AnalyzeNextDiscoveredTile();
	TArray<FIntPoint> GeneratePath() const;
	FPathfindingData PullCheapestTileOutOfDiscoveredList();
	bool DiscoverNextNeighbor();
	void InsertTileIntoDiscoveredArray(const FPathfindingData& TileData);
	bool IsDiagonal(const FIntPoint& Index1, const FIntPoint& Index2) const;
	int32 GetTileSortingCost(const FPathfindingData& Tile) const;

	FIntPoint StartIndex;
	FIntPoint TargetIndex;
	bool bIncludeDiagonalsInPathfinding;
	TArray<FIntPoint> DiscoveredTileIndices;
	TArray<int32> DiscoveredTileSortingCosts;
	TArray<FIntPoint> AnalyzedTileIndices;
	FPathfindingData CurrentDiscoveredTile;
	TMap<FIntPoint, FPathfindingData> PathfindingData;
	TArray<FPathfindingData> CurrentNeighbors;
	FPathfindingData CurrentNeighbor;

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
	TArray<FPathfindingData> CheckPotentialNeighbors(const FIntPoint& Index, TArray<FIntPoint> AttemptedNeighbors) const;
	bool ValidateNeighborIndex(const FTileData& InputTile, const FIntPoint& Neighbor) const;

	/******************************************************************
	 * Delayed Pathfinding
	 ******************************************************************/
public:
	UFUNCTION(BlueprintCallable)
	void FindPathWithDelay();
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float DelayBetweenIterations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxMsPerFrame;

	FDateTime LoopStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bCanCallDelayedPathfinding = false;

	void PerformDelayedPathfinding();
};
