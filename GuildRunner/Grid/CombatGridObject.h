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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Units", meta = (AllowPrivateAccess = "true"))
	FLinearColor SelectedColor = FLinearColor(1, 1, 1, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Units", meta = (AllowPrivateAccess = "true"))
	float HoveredColorMultiplier = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Units", meta = (AllowPrivateAccess = "true"))
	float SelectedColorMultiplier = 2.5f;

protected:
	virtual UMeshComponent* GetMeshComponent() const { return nullptr; }

public:
	void SetGrid(ACombatGrid* Grid) { GridReference = Grid; }

	UFUNCTION(BlueprintCallable)
	FIntPoint GetIndexOnGrid() const { return IndexOnGrid; }
	
	UFUNCTION(BlueprintCallable)
	void SetIndexOnGrid(const FIntPoint Index) { IndexOnGrid = Index; }

	bool GetIsBlockingObject() const { return bBlockingObject; }

	/**
	 * @brief Updates the object's color highlighting when the object is hovered over or selected by the player 
	 */
	UFUNCTION(BlueprintCallable)
	void UpdateVisualIfHoveredOrSelected(UMeshComponent* Mesh) const;

	/**
	 * @brief Sets the object as hovered when the mouse hovers over this object. This purely changes the color of this
	 *	object, all selection logic is handled elsewhere.
	 * @param bHovered Whether this unit is currently hovered by the mouse
	 */
	UFUNCTION(BlueprintCallable)
	void SetIsHovered(const bool bHovered)
	{
		bIsHovered = bHovered;
		UpdateVisualIfHoveredOrSelected(GetMeshComponent());
	}

	/**
	 * @brief Sets the object as selected when the player clicks on this object. This purely changes the color of this
	 *	object, all selection logic is handled elsewhere.
	 * @param bSelected Whether this unit is currently selected by the player
	 */
	UFUNCTION(BlueprintCallable)
	void SetIsSelected(const bool bSelected)
	{
		bIsSelected = bSelected;
		UpdateVisualIfHoveredOrSelected(GetMeshComponent());
	}
	
protected:
	UFUNCTION(BlueprintCallable)
	const ACombatGrid* GetGrid() const { return GridReference; }

	void SetBlockingObject(bool BlockingObject) { bBlockingObject = BlockingObject; }

private:
	UPROPERTY()
	ACombatGrid* GridReference;

	UPROPERTY(VisibleAnywhere)
	FIntPoint IndexOnGrid = FPATHFINDINGDATA_DEFAULT_INDEX;

	UPROPERTY(EditAnywhere)
	bool bBlockingObject = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsHovered;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsSelected;
};