//Copyright(c) 2022 - 2024, Viktor.F.P

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "InventoryUMG/InventoryPanel.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Inventory/InventoryComponent.h"
#include "InventoryGrid.generated.h"

UCLASS()
class INVENTORY_API UInventoryGrid : public UInventoryPanel
{
	GENERATED_UCLASS_BODY()


	enum TypeChangeTranstrorm {
		E_Position,
		E_Size
	};

public:

	UPROPERTY(EditAnywhere, NoClear, Category = "InventoryGrid")
		TSubclassOf<UUserWidget> ItemSlot;
	
	UPROPERTY(EditAnywhere, Category = "InventoryGrid")
		TSubclassOf<UUserWidget> NoneSlot;

	UPROPERTY(EditAnywhere, Category = "InventoryGrid")
		float SizeSlot = 32.0f;

	virtual void SetInventory(UInventoryComponent* NewInventory) override;

	void SetSlotTranstrorm(UInventoryPanelSlot* ChangeSlot, FIntPoint Value, TypeChangeTranstrorm Change);

protected:
	
	void AddNoneSlot(FIntPoint Position);

	void AddSlot(int32 IndexItem);
	
	virtual void OnChangeSlot(int32 Index, FInventorySlot NewData, ETypeSetItem Type) override;

protected:

	UFUNCTION()
		void ChangeSlots(int32 Index, FInventorySlot NewData, ETypeSetItem Type);

};
