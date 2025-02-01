//Copyright(c) 2022 - 2025, Viktor.F.P

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Components/PanelWidget.h"
#include "Widgets/SWidget.h"
#include "Layout/Margin.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Inventory/InventoryComponent.h"
#include "InventoryPanel.generated.h"

class UInventoryPanelSlot;

UCLASS(NotBlueprintable, Abstract)
class INVENTORY_API UInventoryPanel : public UPanelWidget
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = "InventoryPanel")
		TArray<UInventoryPanelSlot*> ItemSlots;

	UPROPERTY()
		UInventoryComponent* Inventory;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

	UFUNCTION(BlueprintCallable, Category = "InventoryPanel")
		virtual void SetInventory(UInventoryComponent* NewInventory);

	UFUNCTION(BlueprintPure, Category = "InventoryPanel|Slot")
		static UInventoryPanelSlot* SlotAsInventorySlot(UWidget* Widget);


	UFUNCTION(BlueprintPure, Category = "InventoryPanel")
		UInventoryComponent* GetInventory() const {
		return Inventory;
	};

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "AddChildToInventoryPanel (Item)"), Category = "Widget")
		UInventoryPanelSlot* AddChildToInventoryPanelItem(UWidget* Content, int32 Index);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "AddChildToInventoryPanel"), Category = "Widget")
		UInventoryPanelSlot* AddChildToInventoryPanel(UWidget* Content);

	void ReleaseSlateResources(bool bReleaseChildren) override;

	virtual void RemoveFromParent() override;

protected:

	UFUNCTION()
		virtual	void OnChangeSlot(int32 Index, FInventorySlot NewData, ETypeSetItem Type) {};

	// UPanelWidget
	virtual UClass* GetSlotClass() const override;
	virtual void OnSlotAdded(UPanelSlot* inSlot) override;
	virtual void OnSlotRemoved(UPanelSlot* inSlot) override;

	TSharedPtr<SConstraintCanvas> MyPanel;

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;

};
