// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ETileState.h"
#include "Components/SceneComponent.h"
#include "CombatGridMeshInstance.generated.h"

UCLASS(meta=(BlueprintSpawnableComponent) )
class GUILDRUNNER_API UCombatGridMeshInstance : public USceneComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|GridMesh", meta = (AllowPrivateAccess = "true"))
	UInstancedStaticMeshComponent* InstancedMesh;
	
public:	
	// Sets default values for this actor's properties
	UCombatGridMeshInstance();

	UFUNCTION(BlueprintCallable)
	void AddInstance(const FIntPoint& Index, const FTransform& Transform, const TArray<TEnumAsByte<ETileState>>& TileStates);

	UFUNCTION(BlueprintCallable)
	void RemoveInstance(const FIntPoint& Index);

	UFUNCTION(BlueprintCallable)
	void ClearInstances();

	UFUNCTION(BlueprintCallable)
	void InitializeGridMeshInstance(UStaticMesh* NewMesh, UMaterialInstance* NewMaterial, FLinearColor NewColor, ECollisionEnabled::Type CollisionType);

	UFUNCTION(BlueprintCallable)
	void AdjustInstanceMeshOffsetFromGround(const float Offset);

	static FLinearColor GetColorFromStates(const TArray<TEnumAsByte<ETileState>>& States, float& IsFilledValue);

private:
	TArray<FIntPoint> InstanceIndices;
};
