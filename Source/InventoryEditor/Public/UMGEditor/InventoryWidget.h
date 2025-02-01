//Copyright(c) 2022 - 2025, Viktor.F.P

#pragma once
#include "CoreMinimal.h"
#include "UMG/Public/Blueprint/UserWidget.h"
#include "Input/Reply.h"
#include "InventoryWidget.generated.h"

UCLASS(HideDropdown)
class INVENTORYEDITOR_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	class UInventoryGrid* InventoryGrid;
	class UInventoryList* InventoryList;
	class UInventoryComponent* Inventory;

	uint8 Panel = 0;

	TSharedPtr<class STextBlock> TextNameActor;
	TSharedPtr<class SHorizontalBox> Recalculation;
	TSharedPtr<class SWidgetSwitcher> Switcher;
	TSharedPtr<class STextBlock> MassText;

	FReply RecalculationMass();

	void SwitchPanel(uint8 NewPanel);

	void SetInventory(UInventoryComponent* NewInventory, uint8 NewPanel);

	virtual void RemoveFromParent() override;

protected:

	class UScaleBox* InventoryGridBox;
	class UScaleBox* InventoryListBox;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	TSharedPtr<SVerticalBox> MyPanel;

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
};