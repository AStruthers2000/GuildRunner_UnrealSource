#pragma once
#include "CoreMinimal.h"
#include "FAbilityStatModifier.generated.h"

UENUM(BlueprintType)
enum class EStatModifierType : uint8
{
	Fixed			UMETA(DisplayName = "Fixed Modification"),
	RandomRange		UMETA(DisplayName = "Random Modification"),
	DnDStyle		UMETA(DisplayName = "DnD Style Modification"),
};

UENUM(BlueprintType)
enum class EDice : uint8
{
	None = 0,
	D4 = 4,
	D6 = 6,
	D8 = 8,
	D10 = 10,
	D12 = 12,
	D20 = 20,
	D100 = 100
};

USTRUCT(BlueprintType)
struct FAbilityStatModifier
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats|Effects", meta = (AllowPrivateAccess = "true"))
	EStatModifierType ModifierValueCalculationType = EStatModifierType::Fixed;

	// Fixed modification amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats|Effects", meta = (AllowPrivateAccess = "true", EditCondition = "ModifierValueCalculationType == EStatModifierType::Fixed", EditConditionHides))
	int32 StatModifier = 0;

	// Modification range. True value will be randomly determined between [x, y]
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats|Effects", meta = (AllowPrivateAccess = "true", EditCondition = "ModifierValueCalculationType == EStatModifierType::RandomRange", EditConditionHides))
	FIntPoint StatModifierRange = {0, 0};

	// Amount of each type of dice to roll
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats|Effects", meta = (AllowPrivateAccess = "true", EditCondition = "ModifierValueCalculationType == EStatModifierType::DnDStyle", EditConditionHides))
	TMap<EDice, uint8> Dice;

	// Flat modifier added to dice roll
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Stats|Effects", meta = (AllowPrivateAccess = "true", EditCondition = "ModifierValueCalculationType == EStatModifierType::DnDStyle", EditConditionHides))
	int32 BaseModifier = 0;
};