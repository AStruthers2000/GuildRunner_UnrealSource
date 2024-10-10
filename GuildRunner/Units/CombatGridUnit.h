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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Units", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Units", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EUnitType> UnitType;
	
public:	
	// Sets default values for this actor's properties
	ACombatGridUnit();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	
};
