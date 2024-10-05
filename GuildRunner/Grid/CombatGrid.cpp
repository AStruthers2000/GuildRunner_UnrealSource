// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatGrid.h"

#include "CombatGridModifier.h"
#include "GridShapes/GridShapeUtilities.h"
#include "GuildRunner/Utilities/GuildRunnerUtilities.h"
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

    bRefreshGrid = false;
	SpawnGrid(GetActorLocation(), GridTileSize, GridTileCount, GridShape);
}


#if WITH_EDITOR
void ACombatGrid::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if(bRefreshGrid)
	{
		SpawnGrid(GetActorLocation(), GridTileSize, GridTileCount, GridShape);
		bRefreshGrid = false;
	}
}
#endif


void ACombatGrid::SpawnGrid(FVector CentralSpawnLocation, FVector SingleTileSize, FVector2D GridDimensions,
                            TEnumAsByte<EGridShape> TileShape, bool bUseEnvironmentForGridSpawning)
{
	//initialize global variables
	GridCenterLocation = CentralSpawnLocation;
	GridTileSize = SingleTileSize;
	GridTileCount = FVector2D(FMath::RoundToInt32(GridDimensions.X), FMath::RoundToInt32(GridDimensions.Y));
	GridShape = TileShape;

	FindGridCenterAndBottomLeft(GridCenterLocation, GridBottomLeftCornerLocation);

	//destroy previous grid
	GridTiles.Empty();
	CombatGridVisual->DestroyGridVisual();

	//create new grid from current selected instance
	const auto* RowData = GetCurrentShapeData();
	if(RowData && GridShape != NoDefinedShape)
	{
		CombatGridVisual->InitializeGridVisual(this);

		TArray<FTransform> InstancesToAdd;
		FTransform TileTransform;

		const auto TileScale = GridTileSize / RowData->MeshSize;
		TileTransform.SetScale3D(TileScale);

		for (int32 x = 0; x < GridTileCount.X; x++)
		{
			//modify for loop because hexagons have to be special
			const auto GridTileY = GridShape == Hexagon ? GridTileCount.Y * 2 : GridTileCount.Y;
			//if GridShape is a hexagon and x is odd, we want to start on index 1. in all other cases, start at index 0
			const auto GridOffsetY = GridShape == Hexagon && x % 2 != 0 ? 1 : 0;
			
			for(int32 y = GridOffsetY; y < GridTileY; y++)
			{
				const auto Index = FIntPoint(x, y);
				
				TileTransform.SetLocation(GetTileLocationFromGridIndex(Index));
				TileTransform.SetRotation(GetTileRotationFromGridIndex(Index).Quaternion());

				if(bUseEnvironmentForGridSpawning)
				{
					FVector GroundLocation;
					const auto HitTileType = TraceForGround(TileTransform.GetLocation(), GroundLocation);
					TileTransform.SetLocation(GroundLocation);
					AddGridTile({Index, HitTileType, TileTransform});
				}
				else
				{
					AddGridTile({Index, Normal, TileTransform});
				}

				if(GridShape == Hexagon) ++y;
			}
		}
	}
}

ETileType ACombatGrid::TraceForGround(const FVector& Location, FVector& Out_HitLocation) const
{
	TArray<FHitResult> Hits;
	const auto TraceStart = Location + FVector(0.f, 0.f, 10000.f);
	const auto TraceEnd = Location - FVector(0.f, 0.f, 10000.f);
	const auto TraceRadius = GridTileSize.X / GridShape == Triangle ? 5.f : GridShape == Hexagon ? 4.f : 3.f;
	FCollisionQueryParams Params;
	Params.bTraceComplex = false;
	Params.AddIgnoredActor(this);
	Params.TraceTag = "Environment Trace";
	
	FCollisionResponseParams Responses;
	const bool bHit = GetWorld() ? GetWorld()->SweepMultiByChannel(
		Hits,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(TraceRadius),
		Params,
		Responses) : false;

	if(Hits.Num() == 0 || bHit == false)
	{
		Out_HitLocation = Location;
		return NoTile;
	}

	ETileType HitTileType = Normal;
	for(const auto& HitResult : Hits)
	{
		const auto* HitGridObject = Cast<ACombatGridModifier>(HitResult.GetActor());
		if(HitGridObject)
		{
			HitTileType = HitGridObject->GetTileType();
		}
		else
		{
			const auto ModifiedZ = FMath::GridSnap(Hits[0].Location.Z, GridTileSize.Z) - TraceRadius;
			Out_HitLocation = FVector(Location.X, Location.Y, ModifiedZ);
		}
	}
	return HitTileType;
}

void ACombatGrid::AddGridTile(const FTileData& TileData)
{
	GridTiles.Add(TileData.Index, TileData);
	CombatGridVisual->UpdateTileVisual(TileData);
}

FVector ACombatGrid::GetTileLocationFromGridIndex(const FVector2D GridIndex) const
{
	FVector2D ScaledIndex = GridIndex;
	//each shape has a different location scale
	switch (GridShape)
	{
	case Square:
		ScaledIndex *= {1.f, 1.f};
		break;
	case Triangle:
		ScaledIndex *= {1.f, 0.5f};
		break;
	case Hexagon:
		ScaledIndex *= {0.75f, 0.5f};
		break;
	case NoDefinedShape: //intentional fallthrough
	default: return {};
	}
	
	const auto TileLocation = GridBottomLeftCornerLocation + GridTileSize * FVector(ScaledIndex.X, ScaledIndex.Y, 0);
	return TileLocation;
}

FRotator ACombatGrid::GetTileRotationFromGridIndex(const FVector2D GridIndex) const
{
	//we only want to rotate triangles
	if(GridShape != Triangle) return FRotator::ZeroRotator;

	const auto XRotation = UGuildRunnerUtilities::IsFloatEven(GridIndex.X) ? 180.f : 0.f;
	const auto YRotation = UGuildRunnerUtilities::IsFloatEven(GridIndex.Y) ? 180.f : 0.f;
	
	return FRotator(0.f, XRotation + YRotation, 0.f);
}

const FGridShapeData* ACombatGrid::GetCurrentShapeData() const
{
	return UGridShapeUtilities::GetShapeData(GridShape);
}

void ACombatGrid::FindGridCenterAndBottomLeft(FVector& Out_Center, FVector& Out_BottomLeft) const
{
	switch (GridShape)
	{
	case Square:
		{
			Out_Center = UGuildRunnerUtilities::SnapVectorToVector(GridCenterLocation, GridTileSize);
			const auto X = UGuildRunnerUtilities::IsFloatEven(GridTileCount.X) ? 0.f : 1.f;
			const auto Y = UGuildRunnerUtilities::IsFloatEven(GridTileCount.Y) ? 0.f : 1.f;
			const auto TileOffset = GridTileCount - FVector2D(X, Y);
			const auto LocationOffset = (TileOffset / 2.f) * FVector2D(GridTileSize.X, GridTileSize.Y);
			Out_BottomLeft = Out_Center - FVector(LocationOffset.X, LocationOffset.Y, 0.f);
			break;
		}
	case Triangle:
		{
			const auto TriangleGridSize = GridTileSize * FVector(2.f, 1.f, 1.f);
			Out_Center = UGuildRunnerUtilities::SnapVectorToVector(GridCenterLocation, TriangleGridSize);
			const auto TileOffset = GridTileCount - FVector2D(1.f, 1.f);
			const auto LocationOffset = (TileOffset / FVector2D(2.f, 4.f)) * FVector2D(GridTileSize.X, GridTileSize.Y);
			const auto TriangleSnap = UGuildRunnerUtilities::SnapVectorToVector(FVector(LocationOffset.X, LocationOffset.Y, 0.f), TriangleGridSize);
			Out_BottomLeft = Out_Center - TriangleSnap;
			break;
		}
	case Hexagon:
		{
			const auto HexagonGridSize = GridTileSize * FVector(1.5f, 1.f, 1.f);
			Out_Center = UGuildRunnerUtilities::SnapVectorToVector(GridCenterLocation, HexagonGridSize);
			const auto LocationOffset = (GridTileCount / FVector2D(3.f, 2.f)) * FVector2D(GridTileSize.X, GridTileSize.Y);
			const auto TriangleSnap = UGuildRunnerUtilities::SnapVectorToVector(FVector(LocationOffset.X, LocationOffset.Y, 0.f), HexagonGridSize);
			Out_BottomLeft = Out_Center - TriangleSnap;
			break;
		}
	case NoDefinedShape: //intentional fallthrough
		default: break;
	}
}


