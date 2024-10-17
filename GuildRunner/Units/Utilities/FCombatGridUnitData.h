#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GuildRunner/Units/EUnitType.h"
#include "FCombatGridUnitData_Assets.h"
#include "FCombatGridUnitData_Stats.h"
#include "FCombatGridUnitData.generated.h"


USTRUCT(BlueprintType)
struct FCombatGridUnitData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EUnitType> UnitType = NoUnitSelected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCombatGridUnitData_Assets Assets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCombatGridUnitData_Stats Stats;
};

