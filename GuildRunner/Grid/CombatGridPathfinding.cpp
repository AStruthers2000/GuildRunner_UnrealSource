// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatGridPathfinding.h"

#include <queue>
#include <set>

#include "CombatGrid.h"
#include "GridShapes/GridShapeUtilities.h"
#include "Kismet/GameplayStatics.h"
//#include "Utilities/FPathfindingData.h"

// Sets default values
UCombatGridPathfinding::UCombatGridPathfinding()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatGridPathfinding::BeginPlay()
{
	Super::BeginPlay();
	GridReference = Cast<ACombatGrid>(UGameplayStatics::GetActorOfClass(this, ACombatGrid::StaticClass()));
}

/******************************************************************
 * A* Pathfinding
 ******************************************************************/

TArray<FIntPoint> UCombatGridPathfinding::FindPath(FIntPoint StartTile, FIntPoint TargetTile, bool bUsingDiagonals,
                                                   const TArray<TEnumAsByte<ETileType>>& ValidTileTypes, float Delay,
                                                   float MaxMs)
{
	ClearGeneratedPathfindingData();

	StartIndex = StartTile;
	TargetIndex = TargetTile;
	bIncludeDiagonalsInPathfinding = bUsingDiagonals;
	ValidWalkableTiles = ValidTileTypes;
	DelayBetweenIterations = Delay;
	bCanCallDelayedPathfinding = false;
	MaxMsPerFrame = MaxMs;
	//bPathfindingReturnReachables = bReturnReachables;
	//PathfindingMaxPathLength = PathLength;

	//if input data is invalid, we don't want to do pathfinding
	if (!IsInputDataValid(false))
	{
		OnPathfindingCompleted.Broadcast({});
		return {};
	}

	//UE_LOG(LogTemp, Display, TEXT("Input to pathfinding is valid"));

	//now do A* pathfinding
	DiscoverTile({
		StartIndex, FPATHFINDINGDATA_DEFAULT_TILE_COST, 0,
		GetMinimumCostBetweenTwoTiles(StartIndex, TargetIndex, bUsingDiagonals)
	});

	if (DelayBetweenIterations <= 0.0f)
	{
		//UE_LOG(LogTemp, Display, TEXT("Instant pathfinding selected"));
		while (DiscoveredTileIndices.Num() > 0)
		{
			if (AnalyzeNextDiscoveredTile())
			{
				const auto Path = GeneratePath();
				//UE_LOG(LogTemp, Display, TEXT("Valid path found to target"));
				OnPathfindingCompleted.Broadcast(Path);
				return Path;
			}
		}

		//UE_LOG(LogTemp, Display, TEXT("No valid path found to target"));
		//we have analyzed every tile in the grid, so there is no possible path from start to end
		//const TArray<FIntPoint> EmptyPath = {};
		//const auto ReturnPath = bPathfindingReturnReachables ? AnalyzedTileIndices : EmptyPath;
		//OnPathfindingCompleted.Broadcast(ReturnPath);
		//return ReturnPath;

		OnPathfindingCompleted.Broadcast({});
		return {};
	}
	else
	{
		//UE_LOG(LogTemp, Display, TEXT("Delayed pathfinding selected"));
		bCanCallDelayedPathfinding = true;
		FindPathWithDelay();
		return {};
	}
}

/**
 * @brief Uses BFS to floodfill from StartTile to find all tiles that this generation source can reach
 * @param StartTile Index of the tile we want to find reachables from
 * @param bUsingDiagonals Are we allowing this reachable generation to move diagonally
 * @param ValidTileTypes Array of valid tile types for this reachable generation to visit
 * @param Range Movement range for this reachable generation
 * @return Array of all valid tiles that are within Range of the StartTile
 */
TArray<FIntPoint> UCombatGridPathfinding::GetReachableTiles(FIntPoint StartTile, bool bUsingDiagonals,
                                                            const TArray<TEnumAsByte<ETileType>>& ValidTileTypes, int32 Range)
{
	ClearGeneratedPathfindingData();
	
	StartIndex = StartTile;
	TargetIndex = FPATHFINDINGDATA_DEFAULT_INDEX;
	bIncludeDiagonalsInPathfinding = bUsingDiagonals;
	ValidWalkableTiles = ValidTileTypes;

	//we clicked on an invalid tile or the tile we clicked on isn't walkable
	if (!IsInputDataValid(true))
	{
		OnReachableTilesCompleted.Broadcast({});
		return {};
	}

	//we now want to do BFS to find every tile within range of the valid StartTile

	struct FTileNode
	{
		FIntPoint Tile;
		int32 Cost;

		FTileNode()
			: Tile(FPATHFINDINGDATA_DEFAULT_INDEX), Cost(0) {}

		FTileNode(const FIntPoint& InTile, int32 InCost)
			: Tile(InTile), Cost(InCost) {}
	};

	TQueue<FTileNode> Queue;
	TMap<FIntPoint, int32> TileCostMap;

	Queue.Enqueue(FTileNode(StartTile, 0));
	TileCostMap.Add(StartTile, 0);

	while (!Queue.IsEmpty())
	{
		FTileNode TileNode;// = Queue.Peek();
		Queue.Dequeue(TileNode);

		if (TileNode.Cost > Range)
		{
			continue;
		}

		const auto Neighbors = GetValidTileNeighbors(TileNode.Tile, bIncludeDiagonalsInPathfinding);
		for (const auto& NeighborData : Neighbors)
		{
			auto NeighborIndex = NeighborData.Index;
			const auto NeighborInfo = GridReference->GetGridTiles().FindRef(NeighborIndex);

			//calculate the cost scaled for diagonal movement
			int32 NewCost = TileNode.Cost + CalculateCostToEnterTile(NeighborInfo);

			if (NewCost <= Range)
			{
				//We haven't visited this tile, and the cost to move from start to this tile is within range
				if (!TileCostMap.Contains(NeighborIndex))
				{
					Queue.Enqueue(FTileNode(NeighborIndex, NewCost));
					TileCostMap.Add(NeighborIndex, NewCost);
				}

				else if (NewCost < TileCostMap[NeighborIndex])
				{
					TileCostMap[NeighborIndex] = NewCost;
					Queue.Enqueue(FTileNode(NeighborIndex, NewCost));
				}
			}
		}
	}

	TArray<FIntPoint> ReachableTiles;
	for (const auto& [Tile, Cost] : TileCostMap)
	{
		ReachableTiles.Add(Tile);
	}
	OnReachableTilesCompleted.Broadcast(ReachableTiles);
	return ReachableTiles;
}

void UCombatGridPathfinding::ClearGeneratedPathfindingData()
{
	UE_LOG(LogTemp, Display,
	       TEXT("[UCombatGridPathfinding::ClearGeneratedPathfindingData]: Clearing pathfinding data"));
	PathfindingData.Empty();
	DiscoveredTileSortingCosts.Empty();
	DiscoveredTileIndices.Empty();
	AnalyzedTileIndices.Empty();
	CurrentNeighbors.Empty();

	OnPathfindingDataCleared.Broadcast();
}

bool UCombatGridPathfinding::IsInputDataValid(bool bGeneratingReachables) const
{
	//if we are pathfinding to the tile we clicked on, we don't need to do pathfinding
	if (StartIndex == TargetIndex)
	{
		return false;
	}

	//if the start tile or the end tile aren't actually walkable tiles, we don't need to do pathfinding
	if (!GridReference->IsTileWalkable(StartIndex))
	{
		return false;
	}

	//we only care about validating the target if we are not trying to gather the reachables
	if(!bGeneratingReachables)
	{
		if (!GridReference->IsTileWalkable(TargetIndex))
		{
			return false;
		}

	//if (GetMinimumCostBetweenTwoTiles(StartIndex, TargetIndex, bIncludeDiagonalsInPathfinding) <= PathfindingMaxPathLength)
	//{
		//if the target tile isn't a tile that this unit can walk on, or a tile with a unit on it, we don't need pathfinding
		const auto TargetTile = GridReference->GetGridTiles().Find(TargetIndex);
		if (!TargetTile || !ValidWalkableTiles.Contains(TargetTile->Type))
		{
			return false;
		}
		//if (!TargetTile || TargetTile->UnitOnTile)
		//{
		//	return false;
		//}
	//}
	//else
	//{
	//	return false;
	//}
	}

	return true;
}

void UCombatGridPathfinding::DiscoverTile(const FPathfindingData& TilePathData)
{
	PathfindingData.Add(TilePathData.Index, TilePathData);
	InsertTileIntoDiscoveredArray(TilePathData);

	OnPathfindingDataUpdated.Broadcast(TilePathData.Index);
}

int UCombatGridPathfinding::GetMinimumCostBetweenTwoTiles(const FIntPoint& Index1, const FIntPoint& Index2,
                                                          const bool bUsingDiagonals) const
{
	const int32 XComp = (Index1 - Index2).X;
	const int32 YComp = (Index1 - Index2).Y;
	switch (GridReference->GetGridShape())
	{
	case Square:
		if (bUsingDiagonals)
		{
			return FMath::Max(FMath::Abs(XComp), FMath::Abs(YComp));
		}
		else
		{
			return FMath::Abs(XComp) + FMath::Abs(YComp);
		}
	case Hexagon:
		return FMath::Abs(XComp) + FMath::Max((FMath::Abs(YComp) - FMath::Abs(XComp)) / 2, 0);
	case Triangle:
		{
			if (bUsingDiagonals)
			{
				return FMath::Abs(XComp) + FMath::Abs(YComp);
			}

			const int32 AbsX = FMath::Abs(XComp);
			const int32 AbsY = FMath::Abs(YComp);
			const bool FirstComp = Index2.X < Index1.X ? AbsX <= AbsY : AbsX - 1 <= AbsY;
			const bool SecondComp = Index2.X < Index1.X ? AbsX - 1 <= AbsY : AbsX <= AbsY;
			const bool FacingUp = Index1.X % 2 == Index1.Y % 2;
			const bool PartOfNormalZone = FacingUp ? FirstComp : SecondComp;
			if (PartOfNormalZone)
			{
				return AbsX + AbsY;
			}

			if (FacingUp)
			{
				return AbsX * 2;
			}

			const int32 BelowStarting = Index2.X < Index1.X ? -1 : 1;
			const int32 StartFacingUp = FacingUp ? BelowStarting : -BelowStarting;
			return AbsX * 2 + StartFacingUp;
		}
	case NoDefinedShape:
	default:
		return 0;
	}
}

bool UCombatGridPathfinding::AnalyzeNextDiscoveredTile()
{
	CurrentDiscoveredTile = PullCheapestTileOutOfDiscoveredList();
	OnPathfindingDataUpdated.Broadcast(CurrentDiscoveredTile.Index);
	CurrentNeighbors = GetValidTileNeighbors(CurrentDiscoveredTile.Index, bIncludeDiagonalsInPathfinding);

	//check if any of the current neighbors are the target tile
	while (CurrentNeighbors.Num() > 0)
	{
		//we found the target tile
		if (DiscoverNextNeighbor())
		{
			return true;
		}
	}

	//we did not find the target tile, so continue with next discovered tile
	return false;
}

TArray<FIntPoint> UCombatGridPathfinding::GeneratePath() const
{
	FIntPoint Current = TargetIndex;
	TArray<FIntPoint> InvertedPath;

	//loop until we backtrack to start index
	while (Current != StartIndex)
	{
		InvertedPath.Add(Current);
		const auto Next = PathfindingData.FindRef(Current);
		Current = Next.PreviousTile;
	}

	TArray<FIntPoint> InvertedInvertedPath;

	for (int i = InvertedPath.Num() - 1; i >= 0; --i)
	{
		InvertedInvertedPath.Add(InvertedPath[i]);
	}

	//while (InvertedPath.Num() > 0)
	//{
	//	InvertedInvertedPath.Push(InvertedPath.Pop());
	//}

	return InvertedInvertedPath;
}

FPathfindingData UCombatGridPathfinding::PullCheapestTileOutOfDiscoveredList()
{
	//cheapest tile is the first index
	const auto TileIndex = DiscoveredTileIndices[0];

	//remove first element (aka cheapest) in both arrays
	DiscoveredTileSortingCosts.RemoveAt(0);
	DiscoveredTileIndices.RemoveAt(0);

	//order inside analyzed tile indices list doesn't matter
	AnalyzedTileIndices.Add(TileIndex);

	return PathfindingData.FindRef(TileIndex);
}

bool UCombatGridPathfinding::DiscoverNextNeighbor()
{
	CurrentNeighbor = CurrentNeighbors[0];
	CurrentNeighbors.RemoveAt(0);

	//if we have already analyzed this index, we don't want to reanalyze this tile
	if (AnalyzedTileIndices.Contains(CurrentNeighbor.Index))
	{
		return false;
	}

	const int32 CostFromStart = CurrentDiscoveredTile.CostFromStart + CurrentNeighbor.CostToEnterTile;
	//if(CostFromStart <= PathfindingMaxPathLength)
	//{
	const int32 IndexInDiscovered = DiscoveredTileIndices.Find(CurrentNeighbor.Index);

	if (IndexInDiscovered != INDEX_NONE)
	{
		CurrentNeighbor = PathfindingData.FindRef(CurrentNeighbor.Index);

		//this neighbor isn't good enough to continue pathfinding with this neighbor
		if (CostFromStart >= CurrentNeighbor.CostFromStart)
		{
			return false;
		}

		DiscoveredTileSortingCosts.RemoveAt(IndexInDiscovered);
		DiscoveredTileIndices.RemoveAt(IndexInDiscovered);
	}
	FPathfindingData NewTileToDiscover;
	NewTileToDiscover.Index = CurrentNeighbor.Index;
	NewTileToDiscover.CostToEnterTile = CurrentNeighbor.CostToEnterTile;
	NewTileToDiscover.CostFromStart = CostFromStart;
	NewTileToDiscover.MinimumCostToTarget = GetMinimumCostBetweenTwoTiles(
		CurrentNeighbor.Index, TargetIndex, bIncludeDiagonalsInPathfinding);
	NewTileToDiscover.PreviousTile = CurrentDiscoveredTile.Index;
	DiscoverTile(NewTileToDiscover);

	return CurrentNeighbor.Index == TargetIndex;
	//}
	//else
	//{
	//	return false;
	//}
}

void UCombatGridPathfinding::InsertTileIntoDiscoveredArray(const FPathfindingData& TileData)
{
	const int32 SortingCost = GetTileSortingCost(TileData);

	//if there are no elements in our discovered list, or if our cost is larger than the highest cost in the sorted
	//DiscoveredTiles list, then it is already sorted so we just add the cost
	if (DiscoveredTileSortingCosts.Num() == 0 || SortingCost >= DiscoveredTileSortingCosts.Last())
	{
		DiscoveredTileSortingCosts.Add(SortingCost);
		DiscoveredTileIndices.Add(TileData.Index);
	}
	else
	{
		//we have to sort an array. what is this, CS 301???
		//using insert sort with early return
		for (int i = 0; i < DiscoveredTileSortingCosts.Num(); ++i)
		{
			const int Cost = DiscoveredTileSortingCosts[i];
			if (Cost < SortingCost)
			{
				continue;
			}

			DiscoveredTileSortingCosts.Insert(SortingCost, i);
			DiscoveredTileIndices.Insert(TileData.Index, i);
			break;
		}
	}
}

bool UCombatGridPathfinding::IsDiagonal(const FIntPoint& Index1, const FIntPoint& Index2) const
{
	//Get all of Index1's neighbors that are NOT diagonals, then see if Index2 is one of them. If Index2 is in Index1's
	//non-diagonal neighbors, then Index2 is not diagonal from Index1
	const auto NonDiagonals = GetValidTileNeighbors(Index1, false);
	TArray<FIntPoint> NonDiagonalIndices;
	for (const auto& Data : NonDiagonals)
	{
		NonDiagonalIndices.Add(Data.Index);
	}
	const bool ContainsIndex2 = NonDiagonalIndices.Contains(Index2);
	return !ContainsIndex2;
}

int32 UCombatGridPathfinding::GetTileSortingCost(const FPathfindingData& Tile) const
{
	const auto NeighborInfo = GridReference->GetGridTiles().FindRef(Tile.Index);
	int32 CostToEnter = CalculateCostToEnterTile(NeighborInfo) * COMBAT_GRID_NORMAL_COST;
	
	int32 SortingCost = (Tile.CostFromStart + Tile.MinimumCostToTarget) * COMBAT_GRID_NORMAL_COST;
	SortingCost += IsDiagonal(Tile.Index, Tile.PreviousTile) ? COMBAT_GRID_DIAG_COST : 0;
	SortingCost += CostToEnter;
	return SortingCost;
}


/******************************************************************
 * Neighbor Checking
 ******************************************************************/

TArray<FPathfindingData> UCombatGridPathfinding::GetValidTileNeighbors(const FIntPoint Index,
                                                                       const bool bIncludeDiagonals) const
{
	switch (GridReference->GetGridShape())
	{
	case Square:
		return GetNeighborIndicesForSquare(Index, bIncludeDiagonals);
	case Hexagon:
		return GetNeighborIndicesForHexagon(Index);
	case Triangle:
		return GetNeighborIndicesForTriangle(Index, bIncludeDiagonals);
	case NoDefinedShape: //intentional fallthrough
	default: return {};
	}
}

TArray<FPathfindingData> UCombatGridPathfinding::GetNeighborIndicesForSquare(
	const FIntPoint& Index, const bool bIncludeDiagonals) const
{
	// Define the possible directions for neighbors
	TArray<FIntPoint> AttemptedIndices = {
		{0, 1}, // Up
		{1, 0}, // Right
		{0, -1}, // Down
		{-1, 0} // Left
	};

	// Include diagonal directions if specified
	if (bIncludeDiagonals)
	{
		AttemptedIndices.Add({1, 1}); // Top-right
		AttemptedIndices.Add({1, -1}); // Bottom-right
		AttemptedIndices.Add({-1, 1}); // Top-left
		AttemptedIndices.Add({-1, -1}); // Bottom-left
	}

	return CheckPotentialNeighbors(Index, AttemptedIndices);
}

TArray<FPathfindingData> UCombatGridPathfinding::GetNeighborIndicesForHexagon(const FIntPoint& Index) const
{
	// Define the possible directions for neighbors
	const TArray<FIntPoint> AttemptedIndices = {
		{1, 1},
		{0, 2},
		{-1, 1},
		{-1, -1},
		{0, -2},
		{1, -1}
	};

	return CheckPotentialNeighbors(Index, AttemptedIndices);
}

TArray<FPathfindingData> UCombatGridPathfinding::GetNeighborIndicesForTriangle(
	const FIntPoint& Index, const bool bIncludeDiagonals) const
{
	// Define the possible directions for neighbors
	TArray<FIntPoint> AttemptedIndices = {
		{0, -1},
		{0, 1}
	};

	//if both indices are even or both are odd, the triangle is facing up
	if (Index.X % 2 == Index.Y % 2)
	{
		AttemptedIndices.Add({-1, 0});
		if (bIncludeDiagonals)
		{
			AttemptedIndices.Add({1, 0});
			AttemptedIndices.Add({-1, 2});
			AttemptedIndices.Add({-1, -2});
		}
	}
	else
	{
		AttemptedIndices.Add({1, 0});
		if (bIncludeDiagonals)
		{
			AttemptedIndices.Add({-1, 0});
			AttemptedIndices.Add({1, 2});
			AttemptedIndices.Add({1, -2});
		}
	}

	return CheckPotentialNeighbors(Index, AttemptedIndices);
}

TArray<FPathfindingData> UCombatGridPathfinding::CheckPotentialNeighbors(
	const FIntPoint& Index, TArray<FIntPoint> AttemptedNeighbors) const
{
	const auto InputTile = GridReference->GetGridTiles().FindRef(Index);

	// Store valid neighboring tiles
	TArray<FPathfindingData> Neighbors;

	// Check all potential neighbors
	for (const auto& Direction : AttemptedNeighbors)
	{
		const FIntPoint Neighbor = {Index.X + Direction.X, Index.Y + Direction.Y};
		if (ValidateNeighborIndex(InputTile, Neighbor, ValidWalkableTiles))
		{
			FPathfindingData TempPathfindingData;
			TempPathfindingData.Index = Neighbor;
			TempPathfindingData.CostToEnterTile = CalculateCostToEnterTile(InputTile);
			TempPathfindingData.PreviousTile = Index;
			Neighbors.Add(TempPathfindingData);
		}
	}

	return Neighbors;
}

bool UCombatGridPathfinding::ValidateNeighborIndex(const FTileData& InputTile, const FIntPoint& Neighbor,
                                                   const TArray<TEnumAsByte<ETileType>>& ValidTileTypes) const
{
	if (!GridReference->IsIndexValid(Neighbor))
	{
		return false;
	}

	const auto* NeighborData = GridReference->GetGridTiles().Find(Neighbor);
	if (!NeighborData)
	{
		return false;
	}


	//if(!UGridShapeUtilities::IsTileTypeWalkable(NeighborData->Type)) return false;
	//if the neighbor isn't a valid walkable tile type (some units can walk on FlyingUnitsOnly, etc.)
	if (!ValidTileTypes.Contains(NeighborData->Type))
	{
		return false;
	}

	//if there is a unit on this tile, we don't count it as valid
	//if (NeighborData->UnitOnTile)
	//{
	//	return false;
	//}

	const float MyHeight = InputTile.Transform.GetLocation().Z;
	const float NeighborHeight = NeighborData->Transform.GetLocation().Z;
	const float HeightDifference = FMath::Abs(MyHeight - NeighborHeight);

	//UE_LOG(LogTemp, Display, TEXT("My index is (%d, %d). The neighbor we are evaluating is at index (%d, %d). My height is %2.4f, their height is at %2.4f, and we have a height difference of %2.4f. One tile in the Z is %2.4f units"),
	//	InputTile.Index.X, InputTile.Index.Y, Neighbor.X, Neighbor.Y, MyHeight, NeighborHeight, HeightDifference, GridReference->GetGridTileSize().Z);

	//if there is more than 1 tile's height separating the tile we clicked on and the current neighbor, it is invalid
	if (HeightDifference > GridReference->GetGridTileSize().Z)
	{
		return false;
	}

	//if none of the checks happened, then this tile must be valid
	return true;
}

int32 UCombatGridPathfinding::CalculateCostToEnterTile(const FTileData& InputTile)
{
	switch (InputTile.Type)
	{
	case Normal:
		return FPATHFINDINGDATA_DEFAULT_TILE_COST;
	case DoubleCost:
		return FPATHFINDINGDATA_DEFAULT_TILE_COST * 2;
	case TripleCost:
		return FPATHFINDINGDATA_DEFAULT_TILE_COST * 3;
	case FlyingUnitsOnly:
		return FPATHFINDINGDATA_DEFAULT_TILE_COST;
	case Obstacle:
	case NoTile:
	default: return FPATHFINDINGDATA_DEFAULT_ROUTING_COST;
	}
}


/******************************************************************
 * Delayed Pathfinding
 ******************************************************************/

void UCombatGridPathfinding::FindPathWithDelay()
{
	if (bCanCallDelayedPathfinding)
	{
		LoopStartTime = FDateTime::Now();
		PerformDelayedPathfinding();
	}
}

void UCombatGridPathfinding::PerformDelayedPathfinding()
{
	if (DiscoveredTileIndices.Num() > 0)
	{
		if (AnalyzeNextDiscoveredTile())
		{
			const auto Path = GeneratePath();
			OnPathfindingCompleted.Broadcast(Path);
		}
		else
		{
			if (MaxMsPerFrame > 0.f)
			{
				if ((FDateTime::Now() - LoopStartTime).GetTotalMilliseconds() < MaxMsPerFrame)
				{
					PerformDelayedPathfinding();
					return;
				}
			}

			FLatentActionInfo ActionInfo;
			ActionInfo.CallbackTarget = this;
			ActionInfo.ExecutionFunction = "FindPathWithDelay";
			ActionInfo.UUID = 54321;
			ActionInfo.Linkage = 0;
			UKismetSystemLibrary::RetriggerableDelay(this, DelayBetweenIterations, ActionInfo);
		}
	}
	else
	{
		//we have analyzed every tile in the grid, so there is no possible path from start to end
		OnPathfindingCompleted.Broadcast({});
	}
}
