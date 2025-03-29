// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatGrid.h"

#include "CombatGridModifier.h"
#include "CombatGridObject.h"
#include "CombatGridPathfinding.h"
#include "Algo/AnyOf.h"
#include "GridShapes/GridShapeUtilities.h"
#include "GuildRunner/Utilities/GuildRunnerUtilities.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utilities/CombatGridVisualizer.h"


// Sets default values
ACombatGrid::ACombatGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	auto* DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Default Root"));
	RootComponent = DefaultRoot;

	CombatGridVisual = CreateDefaultSubobject<UCombatGridVisualizer>(TEXT("Combat Grid Visual"));
	CombatGridVisual->SetupAttachment(RootComponent);

	CombatGridPathfinding = CreateDefaultSubobject<UCombatGridPathfinding>(TEXT("Combat Grid Pathfinding Component"));
	//CombatGridPathfinding->RegisterComponent();
	//CombatGridPathfinding.;

	bRefreshGrid = false;
	SpawnGrid(GetActorLocation(), GridTileSize, GridTileCount, GridShape);
}


/******************************************************************
 * Grid Generation 
 ******************************************************************/

void ACombatGrid::SpawnGrid(FVector CentralSpawnLocation, FVector SingleTileSize, FVector2D GridDimensions,
                            EGridShape TileShape, bool bUseEnvironmentForGridSpawning)
{
	//initialize global variables
	GridCenterLocation = CentralSpawnLocation;
	GridTileSize = SingleTileSize;
	GridTileCount = FVector2D(FMath::RoundToInt32(GridDimensions.X), FMath::RoundToInt32(GridDimensions.Y));
	GridShape = TileShape;

	FindGridCenterAndBottomLeft(GridCenterLocation, GridBottomLeftCornerLocation);

	DestroyGrid();

	//create new grid from current selected instance
	const auto* RowData = GetCurrentShapeData();
	if (RowData && GridShape != EGridShape::NoDefinedShape)
	{
		CombatGridVisual->InitializeGridVisual(this);

		TArray<FTransform> InstancesToAdd;
		FTransform TileTransform;

		TileTransform.SetScale3D(GetTileScale());

		for (int32 x = 0; x < GridTileCount.X; x++)
		{
			//modify for loop because hexagons have to be special
			const auto GridTileY = GridShape == EGridShape::Hexagon ? GridTileCount.Y * 2 : GridTileCount.Y;
			//if GridShape is a hexagon and x is odd, we want to start on index 1. in all other cases, start at index 0
			const auto GridOffsetY = GridShape == EGridShape::Hexagon && x % 2 != 0 ? 1 : 0;

			for (int32 y = GridOffsetY; y < GridTileY; y++)
			{
				const auto Index = FIntPoint(x, y);

				TileTransform.SetLocation(GetTileLocationFromGridIndex(Index));
				TileTransform.SetRotation(GetTileRotationFromGridIndex(Index).Quaternion());

				if (bUseEnvironmentForGridSpawning)
				{
					FVector GroundLocation;
					const auto HitTileType = TraceForGround(TileTransform.GetLocation(), GroundLocation);
					TileTransform.SetLocation(GroundLocation);
					AddGridTile({Index, HitTileType, TileTransform});
				}
				else
				{
					AddGridTile({Index, ETileType::Normal, TileTransform});
				}

				if (GridShape == EGridShape::Hexagon)
				{
					++y;
				}
			}
		}

		OnCombatGridGenerated.Broadcast();
	}
}

void ACombatGrid::AddGridTile(const FTileData& TileData)
{
	GridTiles.Add(TileData.Index, TileData);
	CombatGridVisual->UpdateTileVisual(TileData);
	OnTileDataUpdated.Broadcast(TileData.Index);
}

void ACombatGrid::RemoveGridTile(const FIntPoint& Index)
{
	if (GridTiles.Remove(Index))
	{
		CombatGridVisual->UpdateTileVisual({Index});
		OnTileDataUpdated.Broadcast(Index);
	}
}

void ACombatGrid::DestroyGrid()
{
	//destroy previous grid
	GridTiles.Empty();
	CombatGridVisual->DestroyGridVisual();
	OnCombatGridDestroyed.Broadcast();
}

void ACombatGrid::FindGridCenterAndBottomLeft(FVector& Out_Center, FVector& Out_BottomLeft) const
{
	switch (GridShape)
	{
	case EGridShape::Square:
		{
			Out_Center = UGuildRunnerUtilities::SnapVectorToVector(GridCenterLocation, GridTileSize);
			const auto X = UGuildRunnerUtilities::IsFloatEven(GridTileCount.X) ? 0.f : 1.f;
			const auto Y = UGuildRunnerUtilities::IsFloatEven(GridTileCount.Y) ? 0.f : 1.f;
			const auto TileOffset = GridTileCount - FVector2D(X, Y);
			const auto LocationOffset = (TileOffset / 2.f) * FVector2D(GridTileSize.X, GridTileSize.Y);
			Out_BottomLeft = Out_Center - FVector(LocationOffset.X, LocationOffset.Y, 0.f);
			break;
		}
	case EGridShape::Triangle:
		{
			const auto TriangleGridSize = GridTileSize * FVector(2.f, 1.f, 1.f);
			Out_Center = UGuildRunnerUtilities::SnapVectorToVector(GridCenterLocation, TriangleGridSize);
			const auto TileOffset = GridTileCount - FVector2D(1.f, 1.f);
			const auto LocationOffset = (TileOffset / FVector2D(2.f, 4.f)) * FVector2D(GridTileSize.X, GridTileSize.Y);
			const auto TriangleSnap = UGuildRunnerUtilities::SnapVectorToVector(
				FVector(LocationOffset.X, LocationOffset.Y, 0.f), TriangleGridSize);
			Out_BottomLeft = Out_Center - TriangleSnap;
			break;
		}
	case EGridShape::Hexagon:
		{
			const auto HexagonGridSize = GridTileSize * FVector(1.5f, 1.f, 1.f);
			Out_Center = UGuildRunnerUtilities::SnapVectorToVector(GridCenterLocation, HexagonGridSize);
			const auto LocationOffset = (GridTileCount / FVector2D(3.f, 2.f)) * FVector2D(
				GridTileSize.X, GridTileSize.Y);
			const auto TriangleSnap = UGuildRunnerUtilities::SnapVectorToVector(
				FVector(LocationOffset.X, LocationOffset.Y, 0.f), HexagonGridSize);
			Out_BottomLeft = Out_Center - TriangleSnap;
			break;
		}
	case EGridShape::NoDefinedShape: //intentional fallthrough
	default: break;
	}
}

ETileType ACombatGrid::TraceForGround(const FVector& Location, FVector& Out_HitLocation) const
{
	TArray<FHitResult> Hits;
	const auto TraceStart = Location + FVector(0.f, 0.f, 10000.f);
	const auto TraceEnd = Location - FVector(0.f, 0.f, 100000.f);
	const auto TraceRadius = GridTileSize.X / (GridShape == EGridShape::Triangle ? 5.f : GridShape == EGridShape::Hexagon ? 4.f : 3.f);
	FCollisionQueryParams Params;
	Params.bTraceComplex = false;
	Params.AddIgnoredActor(this);
	Params.TraceTag = "Environment Trace";

	FCollisionResponseParams Responses;
	const bool bHit = GetWorld()
		                  ? GetWorld()->SweepMultiByChannel(
			                  Hits,
			                  TraceStart,
			                  TraceEnd,
			                  FQuat::Identity,
			                  ECC_GameTraceChannel1,
			                  FCollisionShape::MakeSphere(TraceRadius),
			                  Params,
			                  Responses)
		                  : false;

	if (Hits.Num() == 0)
	{
		Out_HitLocation = Location;
		return ETileType::NoTile;
	}

	ETileType HitTileType = ETileType::Normal;
	bool bIsHeightFound = false;
	for (const auto& HitResult : Hits)
	{
		const auto ModifiedZ = FMath::GridSnap(HitResult.Location.Z - TraceRadius, GridTileSize.Z); // - TraceRadius;
		const auto* HitGridObject = Cast<ACombatGridModifier>(HitResult.GetActor());
		if (HitGridObject)
		{
			HitTileType = HitGridObject->GetTileType();
			//UE_LOG(LogTemp, Display, TEXT("Hit grid modifier %s at location %s"), *UEnum::GetValueAsString(HitTileType), *Location.ToString());
			if (HitGridObject->GetUseTileHeight())
			{
				bIsHeightFound = true;
				Out_HitLocation = FVector(Location.X, Location.Y, ModifiedZ);
			}
		}
		else
		{
			if (!bIsHeightFound)
			{
				Out_HitLocation = FVector(Location.X, Location.Y, ModifiedZ);
			}
		}
	}
	return HitTileType;
}


/******************************************************************
 * Grid Utilities
 ******************************************************************/

FVector ACombatGrid::GetTileLocationFromGridIndex(const FVector2D GridIndex) const
{
	FVector2D ScaledIndex = GridIndex;
	//each shape has a different location scale
	switch (GridShape)
	{
	case EGridShape::Square:
		ScaledIndex *= {1.f, 1.f};
		break;
	case EGridShape::Triangle:
		ScaledIndex *= {1.f, 0.5f};
		break;
	case EGridShape::Hexagon:
		ScaledIndex *= {0.75f, 0.5f};
		break;
	case EGridShape::NoDefinedShape: //intentional fallthrough
	default: return {};
	}

	const auto TileLocation = GridBottomLeftCornerLocation + GridTileSize * FVector(ScaledIndex.X, ScaledIndex.Y, 0);
	return TileLocation;
}

FRotator ACombatGrid::GetTileRotationFromGridIndex(const FVector2D GridIndex) const
{
	//we only want to rotate triangles
	if (GridShape != EGridShape::Triangle)
	{
		return FRotator::ZeroRotator;
	}

	const auto XRotation = UGuildRunnerUtilities::IsFloatEven(GridIndex.X) ? 180.f : 0.f;
	const auto YRotation = UGuildRunnerUtilities::IsFloatEven(GridIndex.Y) ? 180.f : 0.f;

	return FRotator(0.f, XRotation + YRotation, 0.f);
}

FVector ACombatGrid::GetTileScale() const
{
	return GridTileSize / GetCurrentShapeData()->MeshSize;
}

void ACombatGrid::AddStateToTile(const FIntPoint& Index, const ETileState State)
{
	auto* Data = GridTiles.Find(Index);
	if (Data)
	{
		if (State == ETileState::Selected)
		{
			Data->TileSelected();
		}
		
		if (Data->States.AddUnique(State) >= 0)
		{
			GridTiles.Add(Data->Index, *Data);
			auto States = GetAllTilesWithState(State);
			States.Add(Data->Index);
			TileStateToIndices.Add(State, States);
			CombatGridVisual->UpdateTileVisual(*Data);
			OnTileStateUpdated.Broadcast(Index);
		}
	}
}

void ACombatGrid::RemoveStateFromTile(const FIntPoint& Index, const ETileState State)
{
	auto* Data = GridTiles.Find(Index);
	if (Data)
	{
		if (State == ETileState::Selected)
		{
			Data->TileDeselected();
		}
		
		if (Data->States.Remove(State))
		{
			GridTiles.Add(Data->Index, *Data);
			auto States = GetAllTilesWithState(State);
			States.Remove(Index);
			TileStateToIndices.Add(State, States);
			CombatGridVisual->UpdateTileVisual(*Data);
			OnTileStateUpdated.Broadcast(Index);
		}
	}
}

void ACombatGrid::IncrementTimesTileHasBeenSelected(const FIntPoint& Index)
{
	auto* Data = GridTiles.Find(Index);
	if (Data)
	{
		Data->IncrementTimesSelected();
	}
}

void ACombatGrid::IncrementTimesTileIsInPath(const FIntPoint& Index)
{
	auto* Data = GridTiles.Find(Index);
	if (Data)
	{
		Data->IncrementTimesInPath();

		if (Data->TimesTileIsInPath > 0)
		{
			AddStateToTile(Index, ETileState::IsInPath);
		}
	}
}

void ACombatGrid::DecrementTimesTileIsInPath(const FIntPoint& Index)
{
	auto* Data = GridTiles.Find(Index);
	if (Data)
	{
		Data->DecrementTimesInPath();

		if (Data->TimesTileIsInPath <= 0)
		{
			RemoveStateFromTile(Index, ETileState::IsInPath);
		}
	}
}

bool ACombatGrid::IsIndexValid(const FIntPoint& Index)
{
	return GridTiles.Contains(Index);
}

TArray<FIntPoint> ACombatGrid::GetAllTilesWithState(ETileState State)
{
	/*
	TArray<FIntPoint> Tiles;
	for(auto& Pair : GridTiles)
	{
		if(Pair.Value.States.Contains(State))
		{
			Tiles.Add(Pair.Value.Index);
		}
	}
	return Tiles;
	*/

	const auto TilesWithState = TileStateToIndices.Find(State);
	if (TilesWithState)
	{
		return *TilesWithState;
	}
	return {};
}

void ACombatGrid::ClearStateFromTiles(ETileState State)
{
	for (auto TileIndex : GetAllTilesWithState(State))
	{
		RemoveStateFromTile(TileIndex, State);
	}
}

EGridShape ACombatGrid::GetGridShape() const
{
	return GridShape;
}

const FGridShapeData* ACombatGrid::GetCurrentShapeData() const
{
	return UGridShapeUtilities::GetShapeData(GridShape);
}

#if WITH_EDITOR
void ACombatGrid::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (bRefreshGrid)
	{
		SpawnGrid(GetActorLocation(), GridTileSize, GridTileCount, GridShape);
		bRefreshGrid = false;
	}
}
#endif


/******************************************************************
 * Mouse Cursor
 ******************************************************************/

FVector ACombatGrid::GetCursorLocationOnGrid(int32 PlayerIndex)
{
	const auto* PC = UGameplayStatics::GetPlayerController(this, PlayerIndex);
	FHitResult MouseTraceResult;

	if (PC->GetHitResultUnderCursor(ECC_GameTraceChannel2, false, MouseTraceResult))
	{
		return MouseTraceResult.Location;
	}

	if (PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, false, MouseTraceResult))
	{
		return MouseTraceResult.Location;
	}

	FVector WorldLocation;
	FVector WorldDirection;
	PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	const auto Plane = UKismetMathLibrary::MakePlaneFromPointAndNormal(GridCenterLocation, FVector(0, 0, 1));
	float T;
	FVector Intersection;
	const auto bIsIntersection = UKismetMathLibrary::LinePlaneIntersection(
		WorldLocation,
		WorldLocation + WorldDirection * 999999.f,
		Plane,
		T,
		Intersection);
	if (bIsIntersection)
	{
		return Intersection;
	}
	return FVector(-999, -999, -999);
}

FIntPoint ACombatGrid::GetTileIndexFromWorldLocation(const FVector Location) const
{
	const auto LocationOnGrid = Location - GridBottomLeftCornerLocation;
	FVector SnappedVector;
	FVector2D SnappedLocationOnGrid;
	FVector2D TempIndex;
	FIntPoint Index;
	switch (GridShape)
	{
	case EGridShape::Square:
		SnappedVector = UGuildRunnerUtilities::SnapVectorToVector(LocationOnGrid, GridTileSize);
		SnappedLocationOnGrid = FVector2D(SnappedVector);
		TempIndex = UKismetMathLibrary::Divide_Vector2DVector2D(SnappedLocationOnGrid, FVector2D(GridTileSize));
		Index = TempIndex.IntPoint();
		break;
	case EGridShape::Hexagon:
		SnappedVector = UGuildRunnerUtilities::SnapVectorToVector(LocationOnGrid * FVector(1.0, 2.0, 1.0),
		                                                          GridTileSize * FVector(0.75, 0.25, 1.0));
		SnappedLocationOnGrid = FVector2D(SnappedVector);
		TempIndex = UKismetMathLibrary::Divide_Vector2DVector2D(SnappedLocationOnGrid,
		                                                        FVector2D(GridTileSize) * FVector2D(0.75, 1.0));
		if (UGuildRunnerUtilities::IsFloatEven(TempIndex.X))
		{
			Index = {
				UKismetMathLibrary::FTrunc(TempIndex.X),
				UKismetMathLibrary::FTrunc(FMath::RoundToInt(TempIndex.Y / 2.0f) * 2.0f)
			};
		}
		else
		{
			Index = {
				UKismetMathLibrary::FTrunc(TempIndex.X),
				UKismetMathLibrary::FTrunc(FMath::FloorToInt(TempIndex.Y / 2.0f) * 2.0f + 1.0f)
			};
		}
		break;
	case EGridShape::Triangle:
		SnappedVector =
			UGuildRunnerUtilities::SnapVectorToVector(LocationOnGrid, GridTileSize / FVector(1.0, 2.0, 1.0));
		SnappedLocationOnGrid = FVector2D(SnappedVector);
		TempIndex = UKismetMathLibrary::Divide_Vector2DVector2D(SnappedLocationOnGrid, FVector2D(GridTileSize));
		Index = (TempIndex * FVector2D(1.0, 2.0)).IntPoint();
		break;
	case ETileType::NoTile:
	default:
		return {-999, -999};
	}

	return Index;
}

FIntPoint ACombatGrid::GetTileIndexUnderCursor(const int32 PlayerIndex)
{
	return GetTileIndexFromWorldLocation(GetCursorLocationOnGrid(PlayerIndex));
}

/******************************************************************
 * Pathfinding
 ******************************************************************/
bool ACombatGrid::IsTileWalkable(const FIntPoint& Index)
{
	const auto Tile = GridTiles.Find(Index);
	if (!Tile)
	{
		return false;
	}

	return UGridShapeUtilities::IsTileTypeWalkable(Tile->Type);
}

bool ACombatGrid::IsTileWalkable(const ACombatGridUnit* Unit, const FIntPoint& Index)
{
	const auto Tile = GridTiles.Find(Index);
	if (!Tile)
	{
		return false;
	}
	
	// if this tile is walkable, we want to make sure that this unit is actually capable of walking on it
	bool bIsThisTileWalkable = IsTileWalkable(Index);

	// if the unit's valid tile types contains this tile's type, then we can walk on it
	bool bCanThisUnitWalkOnTile = Unit->GetUnitData().Stats.ValidTileTypes.Contains(Tile->Type);

	return bIsThisTileWalkable && bCanThisUnitWalkOnTile;
}

/******************************************************************
 * Grid Objects
 ******************************************************************/

void ACombatGrid::RegisterGridObjectWithTile(ACombatGridObject* GridObject, const FIntPoint& Index)
{
	if (!GridObject) return;

	GridObject->SetGrid(this);

	auto* Tile = GridTiles.Find(Index);
	if (Tile)
	{
		Tile->AddObjectToTile(GridObject);
	}
}

void ACombatGrid::UnregisterGridObjectWithTile(ACombatGridObject* GridObject)
{
	if (!GridObject) return;

	const auto Index = GridObject->GetIndexOnGrid();
	GridObject->SetGrid(nullptr);

	auto* Tile = GridTiles.Find(Index);
	if (Tile)
	{
		Tile->RemoveObjectFromTile(GridObject);
	}
}

bool ACombatGrid::IsTileOccupiedByBlockingObject(const FIntPoint& Index)
{
	const auto Tile = GridTiles.Find(Index);
	if (!Tile)
	{
		return false;
	}

	auto bIsAnyBlocking = Algo::AnyOf(Tile->ObjectsOnTile, [](const ACombatGridObject* Object)
	{
		return Object->GetIsBlockingObject();
	});

	return bIsAnyBlocking;
}

ACombatGridUnit* ACombatGrid::TryGetUnitOnTile(const FIntPoint& Index)
{
	const auto* Tile = GridTiles.Find(Index);
	if (!Tile) return nullptr;

	return Tile->TryGetUnitOnTile();
}


