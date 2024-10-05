// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridShapes/FGridShapeData.h"
#include "Utilities/ETileType.h"
#include "GuildRunner/Grid/Utilities/FTileData.h"
#include "CombatGrid.generated.h"

class UCombatGridVisualizer;
class UDataTable;

UCLASS()
class GUILDRUNNER_API ACombatGrid : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Grid", meta = (AllowPrivateAccess = "true"))
	UCombatGridVisualizer* CombatGridVisual;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Grid", meta = (AllowPrivateAccess = "true"))
	FVector GridCenterLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Grid", meta = (AllowPrivateAccess = "true"))
	FVector GridTileSize = {200, 200, 500};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Grid", meta = (AllowPrivateAccess = "true"))
	FVector2D GridTileCount = {10, 10};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Grid", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EGridShape> GridShape = Square;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Grid", meta = (AllowPrivateAccess = "true"))
	bool bRefreshGrid;
	
public:	
	ACombatGrid();
	

private:
	/******************************************************************
	 * Grid Generation 
	 ******************************************************************/
	UFUNCTION(BlueprintCallable)
	void SpawnGrid(FVector CentralSpawnLocation, FVector SingleTileSize, FVector2D GridDimensions, TEnumAsByte<EGridShape> TileShape, bool bUseEnvironmentForGridSpawning = false);
public:
	void AddGridTile(const FTileData& TileData);
	void RemoveGridTile(const FIntPoint& Index);
private:
	void FindGridCenterAndBottomLeft(FVector& Out_Center, FVector& Out_BottomLeft) const;
public:
	ETileType TraceForGround(const FVector& Location, FVector& Out_HitLocation) const;
private:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetGridBottomLeftCornerLocation() const { return GridBottomLeftCornerLocation; }
	FVector GridBottomLeftCornerLocation;

	/******************************************************************
	 * Grid Utilities
	 ******************************************************************/
public:
	FVector GetTileLocationFromGridIndex(FVector2D GridIndex) const;
	FRotator GetTileRotationFromGridIndex(FVector2D GridIndex) const;
	FVector GetTileScale() const;
	FVector GetGridTileSize() const { return GridTileSize; }
	const FGridShapeData* GetCurrentShapeData() const;
	void AddStateToTile(const FIntPoint& Index, ETileState State);
	void RemoveStateFromTile(const FIntPoint& Index, ETileState State);
	bool IsIndexValid(const FIntPoint& Index);
	
	UFUNCTION(BlueprintCallable)
	TMap<FIntPoint, FTileData> GetGridTiles() const { return GridTiles; }
private:
	TMap<FIntPoint, FTileData> GridTiles;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	/******************************************************************
	 * Mouse Cursor
	 ******************************************************************/
	UFUNCTION(BlueprintCallable)
	FVector GetCursorLocationOnGrid(int32 PlayerIndex);
	UFUNCTION(BlueprintCallable)
	FIntPoint GetTileIndexFromWorldLocation(FVector Location);
public:
	UFUNCTION(BlueprintCallable)
	FIntPoint GetTileIndexUnderCursor(int32 PlayerIndex);
private:
	

	
};
