#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FGridShapeData.generated.h"

UENUM(BlueprintType)
enum EGridShape
{
	NoDefinedShape,
	Hexagon,
	Square,
	Triangle
};


USTRUCT(BlueprintType)
struct FGridShapeData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector MeshSize = FVector(100, 100, 100);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* MeshMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* FlatMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* FlatBorderMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* FlatFilledMaterial;
};

