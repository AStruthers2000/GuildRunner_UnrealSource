// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EUnitAnimationState.h"
#include "EUnitType.h"
#include "GameFramework/Actor.h"
#include "GuildRunner/Grid/CombatGridObject.h"
#include "Utilities/FCombatGridUnitData.h"
#include "CombatGridUnit.generated.h"


class UCombatGridUnitMovement;
class ACombatGrid;

UCLASS()
class GUILDRUNNER_API ACombatGridUnit : public ACombatGridObject
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Units", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Units",
		meta = (AllowPrivateAccess = "true", ExposeOnSpawn = "true"))
	TEnumAsByte<EUnitType> UnitType = Warrior;

	

public:
	ACombatGridUnit();
	friend UCombatGridUnitMovement;

	UCombatGridUnitMovement* GetCombatGridUnitMovement() const { return UnitMovement; }

	void SetUnitType(const TEnumAsByte<EUnitType> Type) { UnitType = Type; }

	

	FCombatGridUnitData GetUnitData() const { return UnitData; }

	

protected:
	virtual void BeginPlay() override;

private:
	void ConfigureUnitOnConstruct();

	virtual UMeshComponent* GetMeshComponent() const override { return SkeletalMesh; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GuildRunner|Units", meta = (AllowPrivateAccess = "true"))
	UCombatGridUnitMovement* UnitMovement;
	

	UPROPERTY()
	FCombatGridUnitData UnitData;

	UFUNCTION(BlueprintCallable)
	void SetUnitAnimationIndex(TEnumAsByte<EUnitAnimationState> AnimState);

	

	


#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
