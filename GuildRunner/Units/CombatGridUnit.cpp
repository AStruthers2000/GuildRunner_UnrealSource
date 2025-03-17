// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatGridUnit.h"

#include "CombatGridUnitMovement.h"
#include "UnitAnimationInterface.h"
#include "GuildRunner/Grid/CombatGrid.h"
#include "Utilities/UnitsUtilities.h"

ACombatGridUnit::ACombatGridUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	auto* DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Default Root"));
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Unit Skeletal Mesh"));
	UnitMovement = CreateDefaultSubobject<UCombatGridUnitMovement>(TEXT("Unit Movement Handler"));

	RootComponent = DefaultRoot;
	SkeletalMesh->SetupAttachment(RootComponent);
	SkeletalMesh->SetWorldRotation(FRotator(0, -90, 0));
	SkeletalMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	SetBlockingObject(true);
	
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

	ConfigureUnitOnConstruct();
	SetUnitAnimationIndex(Idle);

	UnitMovement->SetMyUnit(this);
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
