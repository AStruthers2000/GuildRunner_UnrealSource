// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatGridUnit.h"

#include "UnitAnimationInterface.h"
#include "GuildRunner/Grid/CombatGrid.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utilities/UnitsUtilities.h"

ACombatGridUnit::ACombatGridUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	auto* DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Default Root"));
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Unit Skeletal Mesh"));

	RootComponent = DefaultRoot;
	SkeletalMesh->SetupAttachment(RootComponent);
	SkeletalMesh->SetWorldRotation(FRotator(0, -90, 0));
	SkeletalMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	ConfigureUnitOnConstruct();
}

void ACombatGridUnit::ConfigureUnitOnConstruct()
{
	const auto* Data = UUnitsUtilities::GetDefaultUnitDataFromUnitType(UnitType);
	if (!Data || Data->UnitType == NoUnitSelected)
	{
		return;
	}

	UnitData = *Data;

	SkeletalMesh->SetSkeletalMesh(UnitData.Assets.Mesh);
	SkeletalMesh->SetAnimInstanceClass(UnitData.Assets.Animation.LoadSynchronous());
	SkeletalMesh->InitAnim(true);

	

	UE_LOG(LogTemp, Display, TEXT("Spawning unit of type: %s"), *UEnum::GetDisplayValueAsText(UnitType).ToString());
}

void ACombatGridUnit::BeginPlay()
{
	Super::BeginPlay();
	SetBlockingObject(true);

	ConfigureUnitOnConstruct();
	SetUnitAnimationIndex(Idle);

	FOnTimelineVector UpdateFunction;
	UpdateFunction.BindUFunction(this, FName("TimelineUpdate"));
	UnitMovementTimeline.AddInterpVector(UnitMovementCurve, UpdateFunction);

	FOnTimelineEvent FinishFunction;
	FinishFunction.BindUFunction(this, FName("OnTimelineFinished"));
	UnitMovementTimeline.SetTimelineFinishedFunc(FinishFunction);

	UnitMovementTimeline.SetTimelineLength(1.f);
}

void ACombatGridUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UnitMovementTimeline.TickTimeline(DeltaTime);
}

void ACombatGridUnit::UnitFollowPath(const TArray<FIntPoint>& TilesInPath)
{
	if (TilesInPath.Num() > 0)
	{
		CurrentPathToFollow = TilesInPath;
		BeginWalkingForward();
	}
}


void ACombatGridUnit::BeginWalkingForward()
{
	SetUnitAnimationIndex(Walk);
	PreviousTileTransform = GetActorTransform();
	NextTileTransform = GetGrid()->GetGridTiles().Find(CurrentPathToFollow[0])->Transform;
	const auto LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PreviousTileTransform.GetLocation(),
	                                                                   NextTileTransform.GetLocation());
	NextTileTransform.SetRotation(FRotator(0, LookAtRotation.Yaw, 0).Quaternion());
	UnitMovementTimeline.SetPlayRate(
		UKismetMathLibrary::SafeDivide(UnitMovementTimeline.GetTimelineLength(), MoveDurationPerTile));

	UnitMovementTimeline.PlayFromStart();
}

void ACombatGridUnit::ContinueToFollowPath()
{
	if (CurrentPathToFollow.Num() > 0)
	{
		BeginWalkingForward();
	}
	else
	{
		SetUnitAnimationIndex(Idle);
		OnCombatUnitFinishedWalking.Broadcast(this);
	}
}


void ACombatGridUnit::TimelineUpdate(const FVector& Value)
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
	SetActorLocationAndRotation(NewLocation + JumpOffset, NewRotation);
}

void ACombatGridUnit::OnTimelineFinished()
{
	OnCombatUnitReachedNewTile.Broadcast(this, CurrentPathToFollow[0]);
	CurrentPathToFollow.RemoveAt(0);
	ContinueToFollowPath();
}




void ACombatGridUnit::SetUnitAnimationIndex(TEnumAsByte<EUnitAnimationState> AnimState)
{
	if (SkeletalMesh->GetAnimInstance()->Implements<UUnitAnimationInterface>())
	{
		IUnitAnimationInterface::Execute_SetUnitAnimationState(SkeletalMesh->GetAnimInstance(), AnimState);
		//UE_LOG(LogTemp, Display, TEXT("Setting actor %s to animation state %s"), *GetActorNameOrLabel(), *UEnum::GetValueAsString(AnimState));
	}
}


#if WITH_EDITOR
void ACombatGridUnit::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ConfigureUnitOnConstruct();
}
#endif
