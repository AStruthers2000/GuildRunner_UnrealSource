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

TArray<FIntPoint> UCombatGridPathfinding::FindPath(FIntPoint StartTile, FIntPoint TargetTile, bool bUsingDiagonals, bool bCalculatingRange,
                                                   const TArray<TEnumAsByte<ETileType>>& ValidTileTypes, int32 Range)
{
	//ClearGeneratedPathfindingData();

	StartIndex = StartTile;
	TargetIndex = TargetTile;
	ValidWalkableTiles = ValidTileTypes;

	//if input data is invalid, we don't want to do pathfinding
	if (!IsInputDataValid(bCalculatingRange))
	{
		OnPathfindingCompleted.Broadcast({});
		return {};
	}

	//UE_LOG(LogTemp, Display, TEXT("Input to pathfinding is valid"));

	//now do A* pathfinding
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, StartTile, TargetTile, bUsingDiagonals, Range]()
	{
		ExecuteAsyncPathfinding(StartTile, TargetTile, bUsingDiagonals, Range);
	});

	return {};
}

//void UCombatGridPathfinding::ClearGeneratedPathfindingData()
//{
	//UE_LOG(LogTemp, Display,
	//       TEXT("[UCombatGridPathfinding::ClearGeneratedPathfindingData]: Clearing pathfinding data"));
	//PathfindingData.Empty();
	//DiscoveredTileSortingCosts.Empty();
	//DiscoveredTileIndices.Empty();
	//AnalyzedTileIndices.Empty();
	//CurrentNeighbors.Empty();

	//OnPathfindingDataCleared.Broadcast();
//}

void UCombatGridPathfinding::ExecuteAsyncPathfinding(const FIntPoint& StartTile, const FIntPoint& TargetTile,
	bool bUsingDiagonals, const int32& Range) const
{
	TSet<FIntPoint> ClosedSet;
	TArray<TSharedPtr<FPathfindingNode>> OpenList;

	OpenList.Add(MakeShared<FPathfindingNode>(StartTile, 0, GetMinimumCostBetweenTwoTiles(StartTile, TargetTile, bUsingDiagonals)));

	TMap<FIntPoint, TSharedPtr<FPathfindingNode>> NodeMap;

	while (OpenList.Num() > 0)
	{
		OpenList.Sort([this](const TSharedPtr<FPathfindingNode>& A, const TSharedPtr<FPathfindingNode>& B)
		{
			auto InlineDiagonal = [](const FIntPoint& From, const FIntPoint& To) -> bool
			{
				return (From.X != To.X) && (From.Y != To.Y);
			};

			//does node A introduce a diagonal movement 
			bool DiagonalA = A->Parent ? InlineDiagonal(A->Index, A->Parent->Index) : false;

			//does node B introduce a diagonal movement
			bool DiagonalB = B->Parent ? InlineDiagonal(B->Index, B->Parent->Index) : false;

			
			int32 ASortingCost = A->FCost() * COMBAT_GRID_NORMAL_COST;
			ASortingCost += DiagonalA ? COMBAT_GRID_DIAG_COST : 0;

			int32 BSortingCost = B->FCost() * COMBAT_GRID_NORMAL_COST;
			BSortingCost += DiagonalB ? COMBAT_GRID_DIAG_COST : 0;
			
			if (ASortingCost != BSortingCost)
			{
				return ASortingCost < BSortingCost;
			}
			
			/*
			 *	 D_A | D_B |  R
			 *	-----+-----+-----
			 *	  0  |  0  |  0
			 *	  0  |  1  |  1
			 *	  1  |  0  |  0
			 *	  1  |  1  |  0
			 * Where:
			 *	0 -> keep A and B's relative order
			 *  1 -> A should come before B 
			 */
			
			return DiagonalA == DiagonalB ? false : !DiagonalA;
		});

		TSharedPtr<FPathfindingNode> CurrentNode = OpenList[0];
		OpenList.RemoveAt(0);
		ClosedSet.Add(CurrentNode->Index);

		if (CurrentNode->Index == TargetTile)
		{			
			TArray<FIntPoint> Path;
			auto Node = CurrentNode;
			while (Node)
			{
				UE_LOG(LogTemp, Display, TEXT("Node at index %s has G cost: %d, H cost: %d, F cost: %d\n"), *Node->Index.ToString(), Node->GCost, Node->HCost, Node->FCost());
				Path.Add(Node->Index);
				Node = Node->Parent;
			}
			Algo::Reverse(Path);

			AsyncTask(ENamedThreads::GameThread, [this, Path]()
			{
				OnPathfindingCompleted.Broadcast(Path);
			});

			return;
		}

		for (const auto& Neighbor : GetValidTileNeighbors(CurrentNode->Index, bUsingDiagonals))
		{
			const auto NeighborIndex = Neighbor.Index;

			if (ClosedSet.Contains(NeighborIndex) || !GridReference->IsTileWalkable(NeighborIndex))
			{
				continue;
			}
			
			const auto NeighborInfo = GridReference->GetGridTiles().FindRef(NeighborIndex);

			int NewGCost = CurrentNode->GCost + CalculateCostToEnterTile(NeighborInfo);

			if (NewGCost <= Range)
			{
				if (!NodeMap.Contains(NeighborIndex) || NewGCost < NodeMap[NeighborIndex]->GCost)
				{
					TSharedPtr<FPathfindingNode> NewNode = MakeShared<FPathfindingNode>(
						NeighborIndex,
						NewGCost,
						GetMinimumCostBetweenTwoTiles(NeighborIndex, TargetTile, bUsingDiagonals),
						CurrentNode);

					NodeMap.Add(NeighborIndex, NewNode);
					OpenList.Add(NewNode);
				}
			}
		}
	}

	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		OnPathfindingCompleted.Broadcast(TArray<FIntPoint>());
	});

	AsyncTask(ENamedThreads::GameThread, [this, ClosedSet]()
	{
		OnReachableTilesCompleted.Broadcast(ClosedSet.Array());
	});
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
