// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "CombatGridUnitMovement.generated.h"

class ACombatGridUnit;
class ACombatGrid;
class UCurveVector;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCombatUnitStartedMovingToNewTile, ACombatGridUnit*, Unit, const FIntPoint&, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCombatUnitReachedNewTile, ACombatGridUnit*, Unit, const FIntPoint&, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCombatUnitFinishedWalking, ACombatGridUnit*, Unit);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class GUILDRUNNER_API UCombatGridUnitMovement : public UActorComponent
{
	GENERATED_BODY()

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
	FCombatUnitStartedMovingToNewTile OnCombatUnitStartedMovingToNewTile;
	
	UPROPERTY(BlueprintAssignable, Category = "Test")
	FCombatUnitReachedNewTile OnCombatUnitReachedNewTile;

	UPROPERTY(BlueprintAssignable, Category = "Test")
	FCombatUnitFinishedWalking OnCombatUnitFinishedWalking;

	UCombatGridUnitMovement();
	friend ACombatGridUnit;
	
	void UnitFollowPath(const TArray<FIntPoint>& TilesInPath);
	void SetMoveDurationPerTile(const float NewDuration) { MoveDurationPerTile = NewDuration; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void SetGridReference(ACombatGrid* Grid) { GridReference = Grid; }
	void SetMyUnit(ACombatGridUnit* Unit) { MyUnit = Unit; }

	UPROPERTY()
	ACombatGrid* GridReference;

	UPROPERTY()
	ACombatGridUnit* MyUnit;
	
	FTimeline UnitMovementTimeline;

	UFUNCTION()
	void TimelineUpdate(const FVector& Value);
	void AdvancePath();

	UFUNCTION()
	void OnTimelineFinished();

	FRotator EndPathfindingRotation;
	FTransform PreviousTileTransform;
	FTransform NextTileTransform;
	TArray<FIntPoint> CurrentPathToFollow;
	void BeginWalkingForward();
	void ContinueToFollowPath();

	bool CanUnitMoveToFinalTile() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MoveDurationPerTile = 1.f;
};
