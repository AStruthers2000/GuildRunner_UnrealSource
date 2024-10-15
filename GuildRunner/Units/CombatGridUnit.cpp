// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatGridUnit.h"

#include "Utilities/FCombatGridUnitData.h"
#include "Utilities/UnitsUtilities.h"

ACombatGridUnit::ACombatGridUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	auto* DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Default Root"));
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Unit Skeletal Mesh"));

	RootComponent = DefaultRoot;
	SkeletalMesh->SetupAttachment(RootComponent);
	SkeletalMesh->SetWorldRotation(FRotator(0, -90, 0));
	
	ConfigureUnitOnConstruct();
	//SkeletalMesh->SetSkeletalMesh()
}

void ACombatGridUnit::BeginPlay()
{
	Super::BeginPlay();
	ConfigureUnitOnConstruct();
}

void ACombatGridUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACombatGridUnit::ConfigureUnitOnConstruct() const
{
	const auto* UnitData = UUnitsUtilities::GetDefaultUnitDataFromUnitType(UnitType);
	if(!UnitData || UnitData->UnitType == NoUnitSelected) return;
	
	SkeletalMesh->SetSkeletalMesh(UnitData->Assets.Mesh);
	SkeletalMesh->SetAnimInstanceClass(UnitData->Assets.Animation.LoadSynchronous());
	SkeletalMesh->InitAnim(true);

	UE_LOG(LogTemp, Display, TEXT("Spawning unit of type: %s"), *UEnum::GetDisplayValueAsText(UnitType).ToString());
}

#if WITH_EDITOR
void ACombatGridUnit::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ConfigureUnitOnConstruct();
}
#endif

