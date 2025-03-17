// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EUnitAnimationState.h"
#include "EUnitType.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "GuildRunner/Grid/CombatGridObject.h"
#include "GuildRunner/Grid/Utilities/FPathfindingData.h"
#include "Utilities/FCombatGridUnitData.h"
#include "CombatGridUnit.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCombatUnitReachedNewTile, ACombatGridUnit*, Unit, const FIntPoint&, Index);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCombatUnitFinishedWalking, ACombatGridUnit*, Unit);

class ACombatGrid;

UCLASS()
class GUILDRUNNER_API ACombatGridUnit : public ACombatGridObject
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Units", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Units",
		meta = (AllowPrivateAccess = "true", ExposeOnSpawn = "true"))
	TEnumAsByte<EUnitType> UnitType = Warrior;

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

	void SetUnitType(const TEnumAsByte<EUnitType> Type) { UnitType = Type; }

	

	FCombatGridUnitData GetUnitData() const { return UnitData; }

	//UFUNCTION(BlueprintImplementableEvent)
	void UnitFollowPath(const TArray<FIntPoint>& TilesInPath);

	
	void SetMoveDurationPerTile(const float NewDuration) { MoveDurationPerTile = NewDuration; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void ConfigureUnitOnConstruct();

	virtual UMeshComponent* GetMeshComponent() const override { return SkeletalMesh; }

	

	

	UPROPERTY()
	FCombatGridUnitData UnitData;

	UFUNCTION(BlueprintCallable)
	void SetUnitAnimationIndex(TEnumAsByte<EUnitAnimationState> AnimState);

	

	FTimeline UnitMovementTimeline;

	UFUNCTION()
	void TimelineUpdate(const FVector& Value);

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
