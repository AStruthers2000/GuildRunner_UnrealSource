// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatGridMeshInstance.h"

#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
UCombatGridMeshInstance::UCombatGridMeshInstance()
{
	PrimaryComponentTick.bCanEverTick = false;

	InstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Instanced Grid Mesh"));
	InstancedMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InstancedMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	InstancedMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
}

void UCombatGridMeshInstance::AddInstance(const FIntPoint& Index, const FTransform& Transform)
{
	//removes the index first before adding it, effectively updating the transform
	RemoveInstance(Index);
	InstancedMesh->AddInstance(Transform);
	InstanceIndices.Add(Index);
}

void UCombatGridMeshInstance::RemoveInstance(const FIntPoint& Index)
{
	if(InstanceIndices.Contains(Index))
	{
		InstancedMesh->RemoveInstance(InstanceIndices.Find(Index));
		InstanceIndices.Remove(Index);
	}
}

void UCombatGridMeshInstance::ClearInstances()
{
	InstancedMesh->ClearInstances();
	InstanceIndices.Empty();
}

void UCombatGridMeshInstance::InitializeGridMeshInstance(UStaticMesh* NewMesh, UMaterialInstance* NewMaterial, const FLinearColor NewColor, const ECollisionEnabled::Type CollisionType)
{
	InstancedMesh->SetStaticMesh(NewMesh);
	InstancedMesh->SetMaterial(0, NewMaterial);
	InstancedMesh->SetVectorParameterValueOnMaterials("Color", FVector(NewColor.R, NewColor.G, NewColor.B));
	InstancedMesh->SetCollisionEnabled(CollisionType);
}

void UCombatGridMeshInstance::AdjustInstanceMeshOffsetFromGround(const float Offset)
{
	const FVector Location = GetComponentLocation();
	InstancedMesh->SetWorldLocation(FVector(Location.X, Location.Y, Offset));
}

