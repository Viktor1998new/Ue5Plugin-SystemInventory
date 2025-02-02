//Copyright(c) 2022 - 2025, Viktor.F.P

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Inventory/InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "InventoryUMG/PanelInventorySlotInterface.h"
#include "SlotsWidget.generated.h"

static const float SizeSlot = 32.f;

UCLASS()
class INVENTORYEDITOR_API UItemSettings : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "AssetData")
	class UItemAsset* Asset;

	UPROPERTY(EditAnywhere, Category = "AssetData", meta = (MultiLine = "true"))
	FString Data;

	UPROPERTY(EditAnywhere, Category = "AssetData", meta = (ClampMin = 1))
	int32 Count;
};

UCLASS(NotBlueprintable, NotBlueprintType, hidedropdown)
class INVENTORYEDITOR_API USlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UInventoryComponent* GetInventory();

protected:
	int32 Item_Index;
	FInventorySlot Item_Slot;


};

UCLASS()
class INVENTORYEDITOR_API USlotNoneWidget : public USlotWidget
{
	GENERATED_BODY()

protected:

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	TSharedPtr<class SConstraintCanvas> MyPanel;

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
};

UCLASS()
class INVENTORYEDITOR_API USlotItemWidget : public USlotWidget, public IPanelInventorySlotInterface
{
	GENERATED_BODY()

	class UInventotySlot_Drag* operation;

public:
	TSharedPtr<class SBorder> ImageItem;
	TSharedPtr<class STextBlock> NumberText;
	TSharedPtr<class SMenuAnchor> MenuAnchor;

	TAttribute<FMargin> A_Offset;
	FVector2D MousePosition;

	UMenuContextItemWidget* ContextMenu;

	FMargin GetOffsetMouse() const;

	void SetVisible();
protected:

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual void OnChangedSlot_Implementation(int32 NewIndex, FInventorySlot NewSlot) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	TSharedPtr<SConstraintCanvas> MyPanel;

	TSharedRef<SWidget> HandleGetMenuContent();

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;

};

UCLASS(NotBlueprintable, NotBlueprintType)
class INVENTORYEDITOR_API UVisualDragWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	TSharedPtr<class SBox> SizeBox;

	void SetItem(class UTexture2D* Texture, FVector2D Size);

protected:

	virtual TSharedRef<SWidget> RebuildWidget() override;
};

UCLASS()
class INVENTORYEDITOR_API USlotAssetWidget : public UUserWidget
{
	GENERATED_BODY()

	class UInventotySlot_Drag* operation;

public:
	class UItemAsset* Asset;

	TSharedPtr<class SImage> ItemIcon;
	TSharedPtr<class STextBlock> NameAsset;

	void SetAssetItem(class UItemAsset* NewAsset);

protected:

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;

};

UCLASS(NotBlueprintable, NotBlueprintType)
class INVENTORYEDITOR_API UMenuContextItemWidget : public UUserWidget
{

	GENERATED_BODY()

public:

	UPROPERTY()
	UItemSettings* ItemSettings;
	UInventoryComponent* Inventory;
	int32 Index;
	TSharedPtr<class SMenuAnchor> Menu;

	void CheckSettingsData(FInventorySlot InventorySlot);

	void SetItem(TSharedPtr<class SMenuAnchor> NewMenu, UInventoryComponent* NewInventory, int32 NewIndex);

	FReply OnClickedRemoveItem();

protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;

};


UCLASS()
class INVENTORYEDITOR_API USlotItemListWidget : public USlotWidget, public IPanelInventorySlotInterface
{
	GENERATED_BODY()

public:
	TSharedPtr<class SBorder> ImageItem;
	TSharedPtr<class STextBlock> NumberText;
	TSharedPtr<class SMenuAnchor> MenuAnchor;

	TAttribute<FMargin> A_Offset;
	FVector2D MousePosition;

	UMenuContextItemWidget* ContextMenu;

	FMargin GetOffsetMouse() const;

protected:

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION(BlueprintNativeEvent, Category = "InventoryPanel|Slot")
	void OnChangedSlot(int32 NewIndex, FInventorySlot NewSlot);

	virtual void OnChangedSlot_Implementation(int32 NewIndex, FInventorySlot NewSlot) override;

	TSharedPtr<class SImage> Image;
	TSharedPtr<class STextBlock> Text_Name;
	TSharedPtr<class STextBlock> Text_Data;

	TSharedRef<SWidget> HandleGetMenuContent();

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;

};