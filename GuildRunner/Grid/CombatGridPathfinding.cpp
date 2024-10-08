// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatGridPathfinding.h"

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

TArray<FIntPoint> UCombatGridPathfinding::FindPath(FIntPoint StartTile, FIntPoint TargetTile, bool bUsingDiagonals)
{
	ClearGeneratedPathfindingData();
	
	StartIndex = StartTile;
	TargetIndex = TargetTile;
	bIncludeDiagonalsInPathfinding = bUsingDiagonals;

	//if input data is invalid, we don't want to do pathfinding
	if(!IsInputDataValid()) return {};

	//now do A* pathfinding
	DiscoverTile({StartIndex, FPATHFINDINGDATA_DEFAULT_TILE_COST, 0, GetMinimumCostBetweenTwoTiles(StartIndex, TargetIndex, bUsingDiagonals)});
	while(DiscoveredTileIndices.Num() > 0)
	{
		if(AnalyzeNextDiscoveredTile())
		{
			return GeneratePath();
		}
	}

	//we have analyzed every tile in the grid, so there is no possible path from start to end
	return {};
}

void UCombatGridPathfinding::ClearGeneratedPathfindingData()
{
	PathfindingData.Empty();
	DiscoveredTileSortingCosts.Empty();
	DiscoveredTileIndices.Empty();
	AnalyzedTileIndices.Empty();

	OnPathfindingDataCleared.Broadcast();
}


bool UCombatGridPathfinding::IsInputDataValid() const
{
	//if we are pathfinding to the tile we clicked on, we don't need to do pathfinding
	if(StartIndex == TargetIndex) return false;
	
	//if the start tile or the end tile aren't actually walkable tiles, we don't need to do pathfinding
	if(!GridReference->IsTileWalkable(StartIndex)) return false;
	if(!GridReference->IsTileWalkable(TargetIndex)) return false;

	return true;
}

void UCombatGridPathfinding::DiscoverTile(const FPathfindingData& TilePathData)
{
	PathfindingData.Add(TilePathData.Index, TilePathData);
	InsertTileIntoDiscoveredArray(TilePathData);

	OnPathfindingDataUpdated.Broadcast(TilePathData.Index);
}

int UCombatGridPathfinding::GetMinimumCostBetweenTwoTiles(const FIntPoint& Index1, const FIntPoint& Index2, const bool bUsingDiagonals) const
{
	const int32 XComp = (Index1 - Index2).X;
	const int32 YComp = (Index1 - Index2).Y;
	switch(GridReference->GetGridShape())
	{
	case Square:
		if(bUsingDiagonals)
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
			if(bUsingDiagonals)
			{
				return FMath::Abs(XComp) + FMath::Abs(YComp);
			}

			const int32 AbsX = FMath::Abs(XComp);
			const int32 AbsY = FMath::Abs(YComp);
			const bool FirstComp = Index2.X < Index1.X ? AbsX <= AbsY : AbsX - 1 <= AbsY;
			const bool SecondComp = Index2.X < Index1.X ? AbsX - 1 <= AbsY : AbsX <= AbsY;
			const bool FacingUp = Index1.X % 2 == Index1.Y % 2;
			const bool PartOfNormalZone = FacingUp ? FirstComp : SecondComp;
			if(PartOfNormalZone)
			{
				return AbsX + AbsY;
			}
			
			if(FacingUp)
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
	CurrentNeighbors = GetValidTileNeighbors(CurrentDiscoveredTile.Index, bIncludeDiagonalsInPathfinding);

	//check if any of the current neighbors are the target tile
	while(CurrentNeighbors.Num() > 0)
	{
		//we found the target tile
		if(DiscoverNextNeighbor())
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
	while(Current != StartIndex)
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
	if(AnalyzedTileIndices.Contains(CurrentNeighbor.Index)) return false;

	const int32 CostFromStart = CurrentDiscoveredTile.CostFromStart + CurrentNeighbor.CostToEnterTile;
	const int32 IndexInDiscovered = DiscoveredTileIndices.Find(CurrentNeighbor.Index);

	if(IndexInDiscovered != INDEX_NONE)
	{
		CurrentNeighbor = PathfindingData.FindRef(CurrentNeighbor.Index);

		//this neighbor isn't good enough to continue pathfinding with this neighbor
		if(CostFromStart >= CurrentNeighbor.CostFromStart) return false;
		
		DiscoveredTileSortingCosts.RemoveAt(IndexInDiscovered);
		DiscoveredTileIndices.RemoveAt(IndexInDiscovered);
	}
	FPathfindingData NewTileToDiscover;
	NewTileToDiscover.Index = CurrentNeighbor.Index;
	NewTileToDiscover.CostToEnterTile = CurrentNeighbor.CostToEnterTile;
	NewTileToDiscover.CostFromStart = CostFromStart;
	NewTileToDiscover.MinimumCostToTarget = GetMinimumCostBetweenTwoTiles(CurrentNeighbor.Index, TargetIndex, bIncludeDiagonalsInPathfinding);
	NewTileToDiscover.PreviousTile = CurrentDiscoveredTile.Index;
	DiscoverTile(NewTileToDiscover);

	return CurrentNeighbor.Index == TargetIndex;
}

void UCombatGridPathfinding::InsertTileIntoDiscoveredArray(const FPathfindingData& TileData)
{
	const int32 SortingCost = GetTileSortingCost(TileData);

	//if there are no elements in our discovered list, or if our cost is larger than the highest cost in the sorted
	//DiscoveredTiles list, then it is already sorted so we just add the cost
	if(DiscoveredTileSortingCosts.Num() == 0 || SortingCost >= DiscoveredTileSortingCosts.Last())
	{
		DiscoveredTileSortingCosts.Add(SortingCost);
		DiscoveredTileIndices.Add(TileData.Index);
	}
	else
	{
		//we have to sort an array. what is this, CS 301???
		//using insert sort with early return
		for(int i = 0; i < DiscoveredTileSortingCosts.Num(); ++i)
		{
			const int Cost = DiscoveredTileSortingCosts[i];
			if(Cost < SortingCost) continue;

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
	for(const auto& Data : NonDiagonals)
	{
		NonDiagonalIndices.Add(Data.Index);
	}
	const bool ContainsIndex2 = NonDiagonalIndices.Contains(Index2);
	return !ContainsIndex2;
}

int32 UCombatGridPathfinding::GetTileSortingCost(const FPathfindingData& Tile) const
{
	int32 SortingCost = (Tile.CostFromStart + Tile.MinimumCostToTarget) * COMBAT_GRID_NORMAL_COST;
	SortingCost += IsDiagonal(Tile.Index, Tile.PreviousTile) ? COMBAT_GRID_DIAG_COST : 0;
	return SortingCost;
}


/******************************************************************
 * Neighbor Checking
 ******************************************************************/

TArray<FPathfindingData> UCombatGridPathfinding::GetValidTileNeighbors(const FIntPoint Index, const bool bIncludeDiagonals) const
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

TArray<FPathfindingData> UCombatGridPathfinding::GetNeighborIndicesForSquare(const FIntPoint& Index, const bool bIncludeDiagonals) const
{
	// Define the possible directions for neighbors
	TArray<FIntPoint> AttemptedIndices = {
		{0, 1},  // Up
		{1, 0},  // Right
		{0, -1}, // Down
		{-1, 0}  // Left
	};
	
	// Include diagonal directions if specified
	if (bIncludeDiagonals) {
		AttemptedIndices.Add({1, 1});   // Top-right
		AttemptedIndices.Add({1, -1});  // Bottom-right
		AttemptedIndices.Add({-1, 1});  // Top-left
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

TArray<FPathfindingData> UCombatGridPathfinding::GetNeighborIndicesForTriangle(const FIntPoint& Index, const bool bIncludeDiagonals) const
{
	// Define the possible directions for neighbors
	TArray<FIntPoint> AttemptedIndices = {
		{0, -1},
		{0, 1}
	};

	//if both indices are even or both are odd, the triangle is facing up
	if(Index.X % 2 == Index.Y % 2)
	{
		AttemptedIndices.Add({-1, 0});
		if(bIncludeDiagonals)
		{
			AttemptedIndices.Add({1, 0});
			AttemptedIndices.Add({-1, 2});
			AttemptedIndices.Add({-1, -2});
		}
	}
	else
	{
		AttemptedIndices.Add({1, 0});
		if(bIncludeDiagonals)
		{
			AttemptedIndices.Add({-1, 0});
			AttemptedIndices.Add({1, 2});
			AttemptedIndices.Add({1, -2});
		}
	}

	return CheckPotentialNeighbors(Index, AttemptedIndices);
}

TArray<FPathfindingData> UCombatGridPathfinding::CheckPotentialNeighbors(const FIntPoint& Index, TArray<FIntPoint> AttemptedNeighbors) const
{

	const auto InputTile = GridReference->GetGridTiles().FindRef(Index);
	
	// Store valid neighboring tiles
	TArray<FPathfindingData> Neighbors;
    
	// Check all potential neighbors
	for (const auto& Direction : AttemptedNeighbors) {
		FIntPoint Neighbor = {Index.X + Direction.X, Index.Y + Direction.Y};
		if (ValidateNeighborIndex(InputTile, Neighbor))
		{
			FPathfindingData TempPathfindingData = {Neighbor};
			TempPathfindingData.PreviousTile = Index;
			Neighbors.Add(TempPathfindingData);
		}
	}

	return Neighbors;
}

bool UCombatGridPathfinding::ValidateNeighborIndex(const FTileData& InputTile, const FIntPoint& Neighbor) const
{
	if(!GridReference->IsIndexValid(Neighbor)) return false;

	const auto* NeighborData = GridReference->GetGridTiles().Find(Neighbor);
	if(!NeighborData) return false;
	
	
	if(!UGridShapeUtilities::IsTileTypeWalkable(NeighborData->Type)) return false;

	const float MyHeight = InputTile.Transform.GetLocation().Z;
	const float NeighborHeight = NeighborData->Transform.GetLocation().Z;
	const float HeightDifference = FMath::Abs(MyHeight - NeighborHeight);

	//UE_LOG(LogTemp, Display, TEXT("My index is (%d, %d). The neighbor we are evaluating is at index (%d, %d). My height is %2.4f, their height is at %2.4f, and we have a height difference of %2.4f. One tile in the Z is %2.4f units"),
	//	InputTile.Index.X, InputTile.Index.Y, Neighbor.X, Neighbor.Y, MyHeight, NeighborHeight, HeightDifference, GridReference->GetGridTileSize().Z);

	//if there is more than 1 tile's height separating the tile we clicked on and the current neighbor, it is invalid
	if(HeightDifference > GridReference->GetGridTileSize().Z) return false;

	//if none of the checks happened, then this tile must be valid
	return true;
}

