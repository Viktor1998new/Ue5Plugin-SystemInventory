//Copyright(c) 2022 - 2025, Viktor.F.P
#pragma once
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Layout/Margin.h"
#include "UObject/Interface.h"
#include "Components/PanelSlot.h"
#include "Inventory/InventoryComponent.h"
#include "InventoryPanelSlot.generated.h"

USTRUCT(BlueprintType)
struct FSlotTransformData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnchorData)
		FMargin Offsets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnchorData)
		FAnchors Anchors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnchorData)
		FVector2D Alignment;

};

class UInventoryPanel;

UCLASS()
class INVENTORY_API UInventoryPanelSlot : public UPanelSlot
{
	GENERATED_UCLASS_BODY()

		UInventoryPanel* ParentPanel;

public:

	UPROPERTY(BlueprintReadOnly, Category = "InventoryPanel|Slot")
		FSlotTransformData Transform;

	UPROPERTY(BlueprintReadOnly, Category = "InventoryPanel|Slot")
		int32 IndexItem;

	UPROPERTY(BlueprintReadOnly, Category = "InventoryPanel|Slot")
		int32 ZOrder = 0;

    void BuildSlot(TSharedRef<SConstraintCanvas> GridPanel);

	UFUNCTION(BlueprintCallable, Category = "InventoryPanel|Slot")
		void SetZOrder(int32 InZOrder);

	UFUNCTION(BlueprintCallable, Category = "InventoryPanel|Slot")
		void SetAnchors(FAnchors Anchors);

	UFUNCTION(BlueprintCallable, Category = "InventoryPanel|Slot")
		void SetIndexItem(int32 NewIndex);

	UFUNCTION(BlueprintCallable, Category = "InventoryPanel|Slot")
		void ChangeSlot(FInventorySlot NewData);

	UFUNCTION(BlueprintCallable, Category = "InventoryPanel|Slot")
		void SetMargin(FMargin NewMargin);

	virtual void SynchronizeProperties() override;

private:

	SConstraintCanvas::FSlot* Slot;

};