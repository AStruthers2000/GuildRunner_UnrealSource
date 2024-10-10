// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatGridUnit.h"

ACombatGridUnit::ACombatGridUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	auto* DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Default Root"));
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Unit Skeletal Mesh"));

	RootComponent = DefaultRoot;
	SkeletalMesh->SetupAttachment(RootComponent);
	SkeletalMesh->SetWorldRotation(FRotator(0, -90, 0));
	//SkeletalMesh->SetSkeletalMesh()
}

void ACombatGridUnit::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACombatGridUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

