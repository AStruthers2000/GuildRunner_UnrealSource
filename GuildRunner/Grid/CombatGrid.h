// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridShapes/FGridShapeData.h"
#include "CombatGrid.generated.h"

class UDataTable;

UCLASS()
class GUILDRUNNER_API ACombatGrid : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Grid", meta = (AllowPrivateAccess = "true"))
	UInstancedStaticMeshComponent* InstancedGridMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Grid", meta = (AllowPrivateAccess = "true"))
	UDataTable* GridDataMappingTable;

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
	// Sets default values for this actor's properties
	ACombatGrid();

private:
	UFUNCTION(BlueprintCallable)
	void SpawnGrid(FVector CentralSpawnLocation, FVector SingleTileSize, FVector2D GridDimensions, TEnumAsByte<EGridShape> TileShape);
	FVector GetTileLocationFromGridIndex(FVector2D GridIndex) const;
	FRotator GetTileRotationFromGridIndex(FVector2D GridIndex) const;


	//UFUNCTION(BlueprintCallable)
	const FGridShapeData* GetCurrentShapeData() const;
	void FindGridCenterAndBottomLeft(FVector& Out_Center, FVector& Out_BottomLeft) const;
	FVector GridBottomLeftCornerLocation;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetGridBottomLeftCornerLocation() const { return GridBottomLeftCornerLocation; }

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};
