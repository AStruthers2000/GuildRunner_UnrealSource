// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerGridActions.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TileSelectionManager.h"
#include "GridActions/GridAction.h"
#include "GuildRunner/Combat/CombatSystem.h"
#include "GuildRunner/Grid/CombatGrid.h"
#include "GuildRunner/Units/CombatGridUnit.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerGridActions::APlayerGridActions()
{
	PrimaryActorTick.bCanEverTick = true;

	GridReference = nullptr;
	TileSelectionManager = CreateDefaultSubobject<UTileSelectionManager>(TEXT("TileSelectionManager"));
}

void APlayerGridActions::BeginPlay()
{
	Super::BeginPlay();
	GridReference = Cast<ACombatGrid>(UGameplayStatics::GetActorOfClass(this, ACombatGrid::StaticClass()));
	if (!GridReference)
	{
		UE_LOG(LogTemp, Error, TEXT("[APlayerGridActions::BeginPlay]:\tNo grid found for player actions"));
	}
	
	CombatSystemReference = Cast<ACombatSystem>(UGameplayStatics::GetActorOfClass(this, ACombatSystem::StaticClass()));
	if (!CombatSystemReference)
	{
		UE_LOG(LogTemp, Error, TEXT("[APlayerGridActions::BeginPlay]:\tNo combat system found for player actions"));
	}

	//binding right and left click to the appropriate callback functions
	if (const auto PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		const auto EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PC->InputComponent);
		const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		if (Subsystem && EnhancedInputComponent)
		{
			Subsystem->AddMappingContext(CombatGridActionsMappingContext, 0);
			EnhancedInputComponent->BindAction(CombatGridLeftClickAction, ETriggerEvent::Triggered, this,
			                                   &APlayerGridActions::ExecuteLeftClickActionOnHoveredTile);
			EnhancedInputComponent->BindAction(CombatGridRightClickAction, ETriggerEvent::Triggered, this,
			                                   &APlayerGridActions::ExecuteRightClickActionOnHoveredTile);
		}
	}
	
	
	//CombatSystemReference->OnGridUnitIndexChanged.AddDynamic(this, &APlayerGridActions::OnUnitGridIndexChanged);
	
	TileSelectionManager->SetGridReference(GridReference);
	TileSelectionManager->SetPlayerGridActionsReference(this);

	SetSelectedActions(PlayerAction_LeftClickOnTile, PlayerAction_LeftClickOnTile);
}

void APlayerGridActions::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TileSelectionManager->UpdateTileUnderCursor();
}

void APlayerGridActions::SelectTile(const FIntPoint& Index) const
{
	TileSelectionManager->TrySelectTile(Index);
}

FIntPoint APlayerGridActions::GetSelectedTile() const
{
	return TileSelectionManager->GetSelectedTile();
}

void APlayerGridActions::DeselectCurrentTile() const
{
	TileSelectionManager->DeselectTile(TileSelectionManager->GetSelectedTile());
}

ACombatGridObject* APlayerGridActions::GetSelectedGridObject() const
{
	return TileSelectionManager->GetSelectedGridObject();
}

void APlayerGridActions::DeselectCurrentObject() const
{
	TileSelectionManager->DeselectObject(TileSelectionManager->GetSelectedGridObject());
}

void APlayerGridActions::NotifyTileSelectionChanged(const FIntPoint& Index) const
{
	OnSelectedTileChanged.Broadcast(Index);
}

void APlayerGridActions::SetSelectedActions(const TSubclassOf<AGridAction> SelectedAction,
                                            const TSubclassOf<AGridAction> DeselectedAction)
{
	LeftClickAction = TrySpawnGridAction(LeftClickAction, SelectedAction);
	RightClickAction = TrySpawnGridAction(RightClickAction, DeselectedAction);
	OnSelectedActionsChanged.Broadcast(LeftClickAction, RightClickAction);
}



void APlayerGridActions::ExecuteLeftClickActionOnHoveredTile(const FInputActionValue& Value)
{
	//if(!GridReference) return;
	//GridReference->AddStateToTile(HoveredTile, Selected);

	TileSelectionManager->UpdateTileUnderCursor();
	if (LeftClickAction)
	{
		LeftClickAction->ExecuteGridAction(TileSelectionManager->GetHoveredTile());
	}
}

void APlayerGridActions::ExecuteRightClickActionOnHoveredTile(const FInputActionValue& Value)
{
	//if(!GridReference) return;
	//GridReference->RemoveStateFromTile(HoveredTile, Selected);

	TileSelectionManager->UpdateTileUnderCursor();
	if (RightClickAction)
	{
		RightClickAction->ExecuteGridAction(TileSelectionManager->GetHoveredTile());
	}
}



AGridAction* APlayerGridActions::TrySpawnGridAction(AGridAction*& ActionObject, TSubclassOf<AGridAction> ActionClass)
{
	//if we have a valid player action, we want to destroy it
	if (ActionObject)
	{
		//Destroy(ActionObject);
		ActionObject->EndPlay(EEndPlayReason::Destroyed);
		ActionObject->Destroy();
		ActionObject = nullptr;
	}
	if (ActionClass)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParameters.bDeferConstruction = true;

		if (auto* SpawnedAction = GetWorld()->SpawnActor<AGridAction>(ActionClass, FVector::ZeroVector,
		                                                              FRotator::ZeroRotator, SpawnParameters))
		{
			SpawnedAction->PlayerGridActions = this;
			SpawnedAction->DispatchBeginPlay();
			return SpawnedAction;
		}
	}
	return nullptr;
}

