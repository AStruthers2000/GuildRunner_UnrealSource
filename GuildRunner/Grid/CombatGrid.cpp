// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatGrid.h"

#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
ACombatGrid::ACombatGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	auto* DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Default Root"));
	InstancedGridMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Instanced Static Mesh"));

	RootComponent = DefaultRoot;
	InstancedGridMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACombatGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACombatGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

