// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatGridPathfinding.h"

#include "CombatGrid.h"
#include "GridShapes/GridShapeUtilities.h"
#include "Kismet/GameplayStatics.h"

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

TArray<FIntPoint> UCombatGridPathfinding::GetValidTileNeighbors(const FIntPoint Index, const bool bIncludeDiagonals) const
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

TArray<FIntPoint> UCombatGridPathfinding::GetNeighborIndicesForSquare(const FIntPoint& Index, const bool bIncludeDiagonals) const
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

TArray<FIntPoint> UCombatGridPathfinding::GetNeighborIndicesForHexagon(const FIntPoint& Index) const
{
	// Define the possible directions for neighbors
	TArray<FIntPoint> AttemptedIndices = {
		{1, 1},
		{0, 2},
		{-1, 1},
		{-1, -1},
		{0, -2},
		{1, -1}
	};

	return CheckPotentialNeighbors(Index, AttemptedIndices);
}

TArray<FIntPoint> UCombatGridPathfinding::GetNeighborIndicesForTriangle(const FIntPoint& Index, const bool bIncludeDiagonals) const
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

TArray<FIntPoint> UCombatGridPathfinding::CheckPotentialNeighbors(const FIntPoint& Index, TArray<FIntPoint> AttemptedNeighbors) const
{

	const auto InputTile = GridReference->GetGridTiles().FindRef(Index);
	
	// Store valid neighboring tiles
	TArray<FIntPoint> Neighbors;
    
	// Check all potential neighbors
	for (const auto& Direction : AttemptedNeighbors) {
		FIntPoint Neighbor = {Index.X + Direction.X, Index.Y + Direction.Y};
		if (ValidateNeighborIndex(InputTile, Neighbor))
		{
			Neighbors.Add(Neighbor);
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
