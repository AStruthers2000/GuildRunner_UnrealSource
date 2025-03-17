#include "CombatGridObject.h"

void ACombatGridObject::UpdateVisualIfHoveredOrSelected(UMeshComponent* Mesh) const
{
	const FLinearColor NewColor = (bIsSelected ? SelectedColor : FLinearColor(1, 1, 1, 1)) * (
		bIsHovered ? HoveredColorMultiplier : 1) * (bIsSelected ? SelectedColorMultiplier : 1);
	const FVector ColorAsVector = FVector(NewColor.R, NewColor.G, NewColor.B);
	//UE_LOG(LogTemp, Display, TEXT("ColorMultiply value for unit %s: %s"), *GetActorNameOrLabel(), *ColorAsVector.ToString());
	Mesh->SetVectorParameterValueOnMaterials("ColorMultiply", ColorAsVector);
}
