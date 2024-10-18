// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GuildRunner/Grid/Utilities/FTileData.h"
#include "CombatGridVisualizer.generated.h"

#define GRID_VISUAL_DEFAULT_MIN_Z 999999.f

class UCombatGridMeshInstance;
class ACombatGrid;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GUILDRUNNER_API UCombatGridVisualizer : public USceneComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|GridVisualization",
		meta = (AllowPrivateAccess = "true"))
	UCombatGridMeshInstance* VisualGridMeshInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|GridVisualization",
		meta = (AllowPrivateAccess = "true"))
	UCombatGridMeshInstance* TacticalGridMeshInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|GridVisualization",
		meta = (AllowPrivateAccess = "true"))
	float GridOffsetFromGround = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|GridVisualization",
		meta = (AllowPrivateAccess = "true"))
	bool bIsTacticalGridShowing;

public:
	// Sets default values for this component's properties
	UCombatGridVisualizer();

	UFUNCTION(BlueprintCallable)
	void InitializeGridVisual(ACombatGrid* Grid);

	UFUNCTION(BlueprintCallable)
	void SetOffsetFromGround(float Offset);

	UFUNCTION(BlueprintCallable)
	void DestroyGridVisual();

	UFUNCTION(BlueprintCallable)
	void UpdateTileVisual(FTileData Data);

	UFUNCTION(BlueprintCallable)
	void SetIsTacticalGridShowing(bool bIsTactical);

private:
	UPROPERTY()
	ACombatGrid* GridReference;
	float GridLowestZ = GRID_VISUAL_DEFAULT_MIN_Z;
	void SetGridLowestZ(const float Z = GRID_VISUAL_DEFAULT_MIN_Z);
	void UpdateTileVisual_Tactical(const FTileData& Data);
	bool bNeedToRegenerateTacticalOnNextEnable = false;
};
