// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GuildRunner/Grid/Utilities/FTileData.h"
#include "CombatGridVisualizer.generated.h"


class UCombatGridMeshInstance;
class ACombatGrid;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUILDRUNNER_API UCombatGridVisualizer : public USceneComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|GridVisualization", meta = (AllowPrivateAccess = "true"))
	UCombatGridMeshInstance* GridMeshInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|GridVisualization", meta = (AllowPrivateAccess = "true"))
	float GridOffsetFromGround = 2.f;

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

private:
	UPROPERTY()
	ACombatGrid* GridReference;
		
};
