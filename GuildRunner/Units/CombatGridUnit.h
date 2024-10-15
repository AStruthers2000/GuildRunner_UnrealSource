// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EUnitType.h"
#include "GameFramework/Actor.h"
#include "CombatGridUnit.generated.h"

UCLASS()
class GUILDRUNNER_API ACombatGridUnit : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Units", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Units", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EUnitType> UnitType = Warrior;
	
public:	
	// Sets default values for this actor's properties
	ACombatGridUnit();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void ConfigureUnitOnConstruct() const;

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	
};
