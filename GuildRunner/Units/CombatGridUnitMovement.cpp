// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatGridUnitMovement.h"

#include "CombatGridUnit.h"
#include "INodeAndChannelMappings.h"
#include "UnitAnimationInterface.h"
#include "GuildRunner/Grid/CombatGrid.h"
#include "Kismet/KismetMathLibrary.h"

UCombatGridUnitMovement::UCombatGridUnitMovement()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCombatGridUnitMovement::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineVector UpdateFunction;
	UpdateFunction.BindUFunction(this, FName("TimelineUpdate"));
	UnitMovementTimeline.AddInterpVector(UnitMovementCurve, UpdateFunction);

	FOnTimelineEvent FinishFunction;
	FinishFunction.BindUFunction(this, FName("OnTimelineFinished"));
	UnitMovementTimeline.SetTimelineFinishedFunc(FinishFunction);

	UnitMovementTimeline.SetTimelineLength(1.f);
}

void UCombatGridUnitMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UnitMovementTimeline.TickTimeline(DeltaTime);
}

void UCombatGridUnitMovement::UnitFollowPath(const TArray<FIntPoint>& TilesInPath)
{
	if (!GridReference)
	{
		SetGridReference(MyUnit->GetGrid());
	}
	
	if (TilesInPath.Num() > 0)
	{
		CurrentPathToFollow = TilesInPath;
		BeginWalkingForward();
	}
}


void UCombatGridUnitMovement::BeginWalkingForward()
{
	// if we have one more tile to move to, and we can't move there because another unit got there first, we want to
	// end the movement early
	if (CurrentPathToFollow.Num() == 1)
	{
		if (!CanUnitMoveToFinalTile())
		{
			AdvancePath();
			return;
		}
	}

	OnCombatUnitStartedMovingToNewTile.Broadcast(MyUnit, CurrentPathToFollow[0]);
	
	MyUnit->SetUnitAnimationIndex(Walk);
	PreviousTileTransform = MyUnit->GetActorTransform();
	NextTileTransform = GridReference->GetGridTiles().Find(CurrentPathToFollow[0])->Transform;
	const auto LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PreviousTileTransform.GetLocation(),
	                                                                   NextTileTransform.GetLocation());
	NextTileTransform.SetRotation(FRotator(0, LookAtRotation.Yaw, 0).Quaternion());
	UnitMovementTimeline.SetPlayRate(
		UKismetMathLibrary::SafeDivide(UnitMovementTimeline.GetTimelineLength(), MoveDurationPerTile));

	UnitMovementTimeline.PlayFromStart();
}

void UCombatGridUnitMovement::ContinueToFollowPath()
{
	if (CurrentPathToFollow.Num() > 0)
	{
		BeginWalkingForward();
	}
	else
	{
		MyUnit->SetUnitAnimationIndex(Idle);
		OnCombatUnitFinishedWalking.Broadcast(MyUnit);
		EndPathfindingRotation = MyUnit->GetActorTransform().GetRotation().Rotator();
	}
}

bool UCombatGridUnitMovement::CanUnitMoveToFinalTile() const
{
	return !GridReference->IsTileOccupiedByBlockingObject(CurrentPathToFollow[0]);
}


void UCombatGridUnitMovement::TimelineUpdate(const FVector& Value)
{
	const float LocationAlpha = Value.X;
	const float RotationAlpha = Value.Y;
	const float JumpAlpha = Value.Z;

	const FVector NewLocation = FMath::Lerp(PreviousTileTransform.GetLocation(), NextTileTransform.GetLocation(),
	                                        LocationAlpha);
	const FRotator NewRotation = UKismetMathLibrary::RLerp(PreviousTileTransform.Rotator(), NextTileTransform.Rotator(),
	                                                       RotationAlpha, true);

	const bool bTilesOnSameHeight = FMath::IsNearlyEqual(PreviousTileTransform.GetLocation().Z,
	                                                     NextTileTransform.GetLocation().Z);
	const FVector JumpOffset = FVector(0, 0, bTilesOnSameHeight ? 0.f : JumpAlpha);
	MyUnit->SetActorLocationAndRotation(NewLocation + JumpOffset, NewRotation);
}

void UCombatGridUnitMovement::AdvancePath()
{
	CurrentPathToFollow.RemoveAt(0);
	ContinueToFollowPath();
}

void UCombatGridUnitMovement::OnTimelineFinished()
{
	OnCombatUnitReachedNewTile.Broadcast(MyUnit, CurrentPathToFollow[0]);
	AdvancePath();
}
