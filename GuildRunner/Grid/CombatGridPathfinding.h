// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Utilities/FPathfindingData.h"
#include "CombatGridPathfinding.generated.h"

struct FTileData;
class ACombatGrid;

UCLASS()
class GUILDRUNNER_API UCombatGridPathfinding : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UCombatGridPathfinding();
	virtual void BeginPlay() override;

	void SetGridPathfindingReference(ACombatGrid* Reference) { GridReference = Reference; }

private:
	UPROPERTY()
	ACombatGrid* GridReference = nullptr;

	/******************************************************************
	 * A* Pathfinding
	 ******************************************************************/
public:
	UFUNCTION(BlueprintCallable)
	TArray<FIntPoint> FindPath(FIntPoint StartTile, FIntPoint TargetTile, bool bUsingDiagonals);

	UFUNCTION(BlueprintCallable)
	void ClearGeneratedPathfindingData();

private:
	bool IsInputDataValid() const;
	void DiscoverTile(FPathfindingData TilePathData);
	static int GetMinimumCostBetweenTwoTiles(const FIntPoint& Index1, const FIntPoint& Index2, bool bUsingDiagonals);
	bool AnalyzeNextDiscoveredTile();
	TArray<FIntPoint> GeneratePath();
	FPathfindingData PullCheapestTileOutOfDiscoveredList();
	bool DiscoverNextNeighbor();
	void InsertTileIntoDiscoveredArray(FPathfindingData TileData);

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
};
