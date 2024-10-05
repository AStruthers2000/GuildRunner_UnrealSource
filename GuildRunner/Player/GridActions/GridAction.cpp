// Fill out your copyright notice in the Description page of Project Settings.


#include "GridAction.h"

#include "GuildRunner/Player/PlayerGridActions.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGridAction::AGridAction()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerGridActions = nullptr;
}

void AGridAction::BeginPlay()
{
	Super::BeginPlay();
	//PlayerGridActions = Cast<APlayerGridActions>(UGameplayStatics::GetActorOfClass(this, APlayerGridActions::StaticClass()));
	//if(!PlayerGridActions)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("[AGridAction::BeginPlay]:\tNo player grid actions found for grid action parent"));
	//}
}

void AGridAction::ExecuteGridAction(FIntPoint TileIndex)
{
}
