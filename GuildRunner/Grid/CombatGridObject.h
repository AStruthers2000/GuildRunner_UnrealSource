#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Utilities/FPathfindingData.h"
#include "CombatGridObject.generated.h"

class ACombatGrid;

UCLASS()
class GUILDRUNNER_API ACombatGridObject : public AActor
{
	GENERATED_BODY()

public:
	void SetGrid(ACombatGrid* Grid) { GridReference = Grid; }

	UFUNCTION(BlueprintCallable)
	FIntPoint GetIndexOnGrid() const { return IndexOnGrid; }
	
	UFUNCTION(BlueprintCallable)
	void SetIndexOnGrid(const FIntPoint Index) { IndexOnGrid = Index; }
	
protected:
	UFUNCTION(BlueprintCallable)
	const ACombatGrid* GetGrid() const { return GridReference; }

private:
	UPROPERTY()
	ACombatGrid* GridReference;

	UPROPERTY(VisibleAnywhere)
	FIntPoint IndexOnGrid = FPATHFINDINGDATA_DEFAULT_INDEX;
};