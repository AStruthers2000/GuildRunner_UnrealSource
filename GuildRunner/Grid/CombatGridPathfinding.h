// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

	UFUNCTION(BlueprintCallable)
	TArray<FIntPoint> GetValidTileNeighbors(FIntPoint Index, bool bIncludeDiagonals) const;

private:
	UPROPERTY()
	ACombatGrid* GridReference = nullptr;

	TArray<FIntPoint> GetNeighborIndicesForSquare(const FIntPoint& Index, bool bIncludeDiagonals) const;
	TArray<FIntPoint> GetNeighborIndicesForHexagon(const FIntPoint& Index) const;
	TArray<FIntPoint> GetNeighborIndicesForTriangle(const FIntPoint& Index, bool bIncludeDiagonals) const;
	TArray<FIntPoint> CheckPotentialNeighbors(const FIntPoint& Index, TArray<FIntPoint> AttemptedNeighbors) const;

	bool ValidateNeighborIndex(const FTileData& InputTile, const FIntPoint& Neighbor) const;
	
};
