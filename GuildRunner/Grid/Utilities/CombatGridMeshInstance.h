// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ETileState.h"
#include "ETileType.h"
#include "Components/SceneComponent.h"
#include "CombatGridMeshInstance.generated.h"

UCLASS(meta=(BlueprintSpawnableComponent) )
class GUILDRUNNER_API UCombatGridMeshInstance : public USceneComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|GridMesh", meta = (AllowPrivateAccess = "true"))
	UInstancedStaticMeshComponent* InstancedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|GridMesh|Colors|States", meta = (AllowPrivateAccess = "true"))
	FLinearColor SelectedColor = FLinearColor(0.392157, 0.094118, 0, 1);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|GridMesh|Colors|States", meta = (AllowPrivateAccess = "true"))
	FLinearColor HoveredColor = FLinearColor(1.0, 0.87450980392156862745098039215686, 0, 1);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|GridMesh|Colors|States", meta = (AllowPrivateAccess = "true"))
	FLinearColor NeighborColor = FLinearColor(243.f/255.f, 58.f/255.f, 106.f/255.f, 1);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|GridMesh|Colors|States", meta = (AllowPrivateAccess = "true"))
	FLinearColor InPathColor = FLinearColor(75.f/255.f, 249.f/255.f, 1, 1);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|GridMesh|Colors|States", meta = (AllowPrivateAccess = "true"))
	FLinearColor DiscoveredColor = FLinearColor(250.f/255.f, 128.f/255.f, 114.f/255.f, 1);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|GridMesh|Colors|States", meta = (AllowPrivateAccess = "true"))
	FLinearColor AnalyzedColor =  FLinearColor(1, 0, 125.f/255.f, 1);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|GridMesh|Colors|TileTypes", meta = (AllowPrivateAccess = "true"))
	FLinearColor NormalCostColor = FLinearColor(178.f/255.f, 190.f/255.f, 181.f/255.f, 1);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|GridMesh|Colors|TileTypes", meta = (AllowPrivateAccess = "true"))
	FLinearColor DoubleCostColor = FLinearColor(129.f/255.f, 133.f/255.f, 137.f/255.f, 1);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|GridMesh|Colors|TileTypes", meta = (AllowPrivateAccess = "true"))
	FLinearColor TripleCostColor = FLinearColor(54.f/255.f, 69.f/255.f, 79.f/255.f, 1);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|GridMesh|Colors|TileTypes", meta = (AllowPrivateAccess = "true"))
	FLinearColor FlyingUnitsOnlyColor = FLinearColor(216.f/255.f, 191.f/255.f, 216.f/255.f, 1);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|GridMesh|Colors|TileTypes", meta = (AllowPrivateAccess = "true"))
	FLinearColor ObstacleColor = FLinearColor(227.f/255.f, 11.f/255.f, 92.f/255.f, 1);
	
	
public:	
	// Sets default values for this actor's properties
	UCombatGridMeshInstance();

	UFUNCTION(BlueprintCallable)
	void AddInstance(const FIntPoint& Index, const TEnumAsByte<ETileType> Type, const FTransform& Transform, const TArray<TEnumAsByte<ETileState>>& TileStates);

	UFUNCTION(BlueprintCallable)
	void RemoveInstance(const FIntPoint& Index);

	UFUNCTION(BlueprintCallable)
	void ClearInstances();

	UFUNCTION(BlueprintCallable)
	void InitializeGridMeshInstance(UStaticMesh* NewMesh, UMaterialInstance* NewMaterial, const bool bColorFromTileType, ECollisionEnabled::Type CollisionType);

	UFUNCTION(BlueprintCallable)
	void AdjustInstanceMeshOffsetFromGround(const float Offset);

	FLinearColor GetColorFromStatesOrTileType(const TArray<TEnumAsByte<ETileState>>& States, const TEnumAsByte<ETileType>& TileType, float& IsFilledValue);

	void HideInstancedStaticMeshInGame(bool bHidden) const;

private:
	TArray<FIntPoint> InstanceIndices;
	bool bIsColorBasedOnTileType = false;
};
