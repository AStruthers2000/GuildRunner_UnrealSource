// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EUnitType.h"
#include "GameFramework/Actor.h"
#include "GuildRunner/Grid/Utilities/FPathfindingData.h"
#include "CombatGridUnit.generated.h"

UCLASS()
class GUILDRUNNER_API ACombatGridUnit : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Units", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Units", meta = (AllowPrivateAccess = "true", ExposeOnSpawn = "true"))
	TEnumAsByte<EUnitType> UnitType = Warrior;
	
public:	
	ACombatGridUnit();

	UFUNCTION(BlueprintCallable)
	void SetIndexOnGrid(const FIntPoint Index) { IndexOnGrid = Index; }

	UFUNCTION(BlueprintCallable)
	FIntPoint GetIndexOnGrid() const { return IndexOnGrid; }

	void SetUnitType(const TEnumAsByte<EUnitType> Type) { UnitType = Type; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void ConfigureUnitOnConstruct() const;

	UPROPERTY(VisibleAnywhere)
	FIntPoint IndexOnGrid = FPATHFINDINGDATA_DEFAULT_INDEX;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	
};
