// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EUnitAnimationState.h"
#include "EUnitType.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "GuildRunner/Grid/Utilities/FPathfindingData.h"
#include "Utilities/FCombatGridUnitData.h"
#include "CombatGridUnit.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCombatUnitReachedNewTile, ACombatGridUnit*, Unit, FIntPoint, Index);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCombatUnitFinishedWalking, ACombatGridUnit*, Unit);

class ACombatGrid;

UCLASS()
class GUILDRUNNER_API ACombatGridUnit : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Units", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Units",
		meta = (AllowPrivateAccess = "true", ExposeOnSpawn = "true"))
	TEnumAsByte<EUnitType> UnitType = Warrior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Units", meta = (AllowPrivateAccess = "true"))
	FLinearColor SelectedColor = FLinearColor(1, 1, 1, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Units", meta = (AllowPrivateAccess = "true"))
	float HoveredColorMultiplier = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Units", meta = (AllowPrivateAccess = "true"))
	float SelectedColorMultiplier = 2.5f;

	UPROPERTY(EditAnywhere, Category = "GuildRunner|Units|Timeline", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* MovementSpeedCurve;

	UPROPERTY(EditAnywhere, Category = "GuildRunner|Units|Timeline", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* RotationSpeedCurve;

	UPROPERTY(EditAnywhere, Category = "GuildRunner|Units|Timeline", meta = (AllowPrivateAccess = "true"))
	UCurveVector* UnitMovementCurve;

public:
	/******************************************************************
	 * Grid Callbacks
	 ******************************************************************/
	UPROPERTY(BlueprintAssignable, Category = "Test")
	FCombatUnitReachedNewTile OnCombatUnitReachedNewTile;

	UPROPERTY(BlueprintAssignable, Category = "Test")
	FCombatUnitFinishedWalking OnCombatUnitFinishedWalking;


	ACombatGridUnit();

	UFUNCTION(BlueprintCallable)
	void SetIndexOnGrid(const FIntPoint Index) { IndexOnGrid = Index; }

	UFUNCTION(BlueprintCallable)
	FIntPoint GetIndexOnGrid() const { return IndexOnGrid; }

	void SetUnitType(const TEnumAsByte<EUnitType> Type) { UnitType = Type; }

	UFUNCTION(BlueprintCallable)
	void UpdateVisualIfHoveredOrSelected() const;

	UFUNCTION(BlueprintCallable)
	void SetIsHovered(const bool bHovered)
	{
		bIsHovered = bHovered;
		UpdateVisualIfHoveredOrSelected();
	}

	UFUNCTION(BlueprintCallable)
	void SetIsSelected(const bool bSelected)
	{
		bIsSelected = bSelected;
		UpdateVisualIfHoveredOrSelected();
	}

	FCombatGridUnitData GetUnitData() const { return UnitData; }

	//UFUNCTION(BlueprintImplementableEvent)
	void UnitFollowPath(const TArray<FIntPoint>& TilesInPath);

	UFUNCTION(BlueprintCallable)
	const ACombatGrid* GetGrid() const { return GridReference; }

	void SetGrid(ACombatGrid* Grid) { GridReference = Grid; }
	void SetMoveDurationPerTile(const float NewDuration) { MoveDurationPerTile = NewDuration; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void ConfigureUnitOnConstruct();

	UPROPERTY(VisibleAnywhere)
	FIntPoint IndexOnGrid = FPATHFINDINGDATA_DEFAULT_INDEX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsHovered;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsSelected;

	UPROPERTY()
	FCombatGridUnitData UnitData;

	UFUNCTION(BlueprintCallable)
	void SetUnitAnimationIndex(TEnumAsByte<EUnitAnimationState> AnimState);

	UPROPERTY()
	ACombatGrid* GridReference;

	FTimeline UnitMovementTimeline;

	UFUNCTION()
	void TimelineUpdate(FVector Value);

	UFUNCTION()
	void OnTimelineFinished();

	FTransform PreviousTileTransform;
	FTransform NextTileTransform;
	TArray<FIntPoint> CurrentPathToFollow;
	void BeginWalkingForward();
	void ContinueToFollowPath();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MoveDurationPerTile = 1.f;


#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
