#pragma once
#include "CoreMinimal.h"
#include "FCombatGridUnitData_Assets.generated.h"


USTRUCT(BlueprintType)
struct FCombatGridUnitData_Assets
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UAnimInstance> Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;
};

