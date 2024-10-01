// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatGrid.generated.h"

UCLASS()
class GUILDRUNNER_API ACombatGrid : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Grid", meta = (AllowPrivateAccess = "true"))
	UInstancedStaticMeshComponent* InstancedGridMesh;
	
public:	
	// Sets default values for this actor's properties
	ACombatGrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
