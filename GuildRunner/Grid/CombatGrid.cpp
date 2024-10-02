// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatGrid.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/DataTable.h"
#include "GridShapes/FGridShapeData.h"

// Sets default values
ACombatGrid::ACombatGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	auto* DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Default Root"));
	InstancedGridMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Instanced Static Mesh"));

	RootComponent = DefaultRoot;
	InstancedGridMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UDataTable> GridShapeObject(TEXT("/Script/Engine.DataTable'/Game/GuildRunner/Grid/GridShapes/DT_GridShapeData.DT_GridShapeData'"));
	
	if (GridShapeObject.Succeeded())
	{
		UE_LOG(LogTemp, Display, TEXT("Found grid shape data table"));
		GridDataMappingTable = GridShapeObject.Object;
	}

	SpawnGrid(GridCenterLocation, GridTileSize, GridTileCount, GridShape);
}

// Called when the game starts or when spawned
void ACombatGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACombatGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACombatGrid::SpawnGrid(FVector CentralSpawnLocation, FVector SingleTileSize, FVector2D GridDimensions,
                            TEnumAsByte<EGridShape> TileShape)
{
	//initialize global variables
	GridCenterLocation = CentralSpawnLocation;
	GridTileSize = SingleTileSize;
	GridTileCount = GridDimensions;
	GridShape = TileShape;

	const auto LocationOffset = (GridTileCount / 2.f) * FVector2D(GridTileSize.X, GridTileSize.Y);
	GridBottomLeftCornerLocation = GridCenterLocation - FVector(LocationOffset.X, LocationOffset.Y, 0.f);

	//destroy previous grid
	InstancedGridMesh->ClearInstances();

	//create new grid from current selected instance
	if(const auto* RowData = GetCurrentShapeData())
	{
		InstancedGridMesh->SetStaticMesh(RowData->FlatMesh);
		InstancedGridMesh->SetMaterial(0, RowData->FlatBorderMaterial);

		for (int x = 0; x < GridTileCount.X; x++)
		{
			for(int y = 0; y < GridTileCount.Y; y++)
			{
				FTransform TileTransform;
				TileTransform.SetScale3D(GridTileSize / RowData->MeshSize);

				const auto TileLocation = GridBottomLeftCornerLocation + GridTileSize * FVector(x, y, 0);
				TileTransform.SetLocation(TileLocation);

				InstancedGridMesh->AddInstance(TileTransform);
			}
		}
	}
}

const FGridShapeData* ACombatGrid::GetCurrentShapeData() const
{
	if(!GridDataMappingTable)
	{
		UE_LOG(LogTemp, Error, TEXT("[ACombatGrid::SpawnGrid]:\tError: no grid data table assigned to this class, cannot spawn grid."));
		return nullptr;
	}

	if(GridShape == NoDefinedShape)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ACombatGrid::SpawnGrid]:\tWarning: No defined shape selected, cannot spawn grid."));
		return nullptr;
	}
	
	const FName RowName = FName(UEnum::GetDisplayValueAsText(GridShape).ToString());
	const FGridShapeData* OutRow = GridDataMappingTable->FindRow<FGridShapeData>(RowName, "Tile Information", true);
	return OutRow;
}

void ACombatGrid::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	SpawnGrid(GridCenterLocation, GridTileSize, GridTileCount, GridShape);
}

