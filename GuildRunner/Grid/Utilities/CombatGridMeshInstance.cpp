// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatGridMeshInstance.h"

#include "ETileState.h"
#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
UCombatGridMeshInstance::UCombatGridMeshInstance()
{
	PrimaryComponentTick.bCanEverTick = false;

	InstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Instanced Grid Mesh"));
	InstancedMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InstancedMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	InstancedMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
	InstancedMesh->NumCustomDataFloats = 4;
}

void UCombatGridMeshInstance::AddInstance(const FIntPoint& Index, const FTransform& Transform, const TArray<TEnumAsByte<ETileState>>& TileStates)
{
	//removes the index first before adding it, effectively updating the transform
	RemoveInstance(Index);
	InstancedMesh->AddInstance(Transform);
	const auto I = InstanceIndices.Add(Index);

	float IsFilledValue = 0.0f;
	const auto Color = GetColorFromStates(TileStates, IsFilledValue);
	InstancedMesh->SetCustomDataValue(I, 0, Color.R);
	InstancedMesh->SetCustomDataValue(I, 1, Color.G);
	InstancedMesh->SetCustomDataValue(I, 2, Color.B);
	InstancedMesh->SetCustomDataValue(I, 3, IsFilledValue);
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

FLinearColor UCombatGridMeshInstance::GetColorFromStates(const TArray<TEnumAsByte<ETileState>>& States, float& IsFilledValue)
{
	IsFilledValue = 0.f;
	if(States.IsEmpty()) return FLinearColor(0, 0, 0, 1);

	//this list is the priority of our colors
	for (const auto& State : {Selected, Hovered})
	{
		if(States.Contains(State))
		{
			switch (State)
			{
			case Selected:
				IsFilledValue = 1.f;
				return FLinearColor(0.392157, 0.094118, 0, 1);
			case Hovered:
				IsFilledValue = 1.f;
				return FLinearColor(1.0, 0.87450980392156862745098039215686, 0, 1);
			case None:
			default: return FLinearColor(0, 0, 0, 1);
			}
		}
	}
	return FLinearColor(0, 0, 0, 1);
}

