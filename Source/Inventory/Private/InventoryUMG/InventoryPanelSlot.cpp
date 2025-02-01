//Copyright(c) 2022 - 2025, Viktor.F.P
#pragma once
#include "InventoryUMG/InventoryPanelSlot.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "InventoryUMG/PanelInventorySlotInterface.h"
#include "InventoryUMG/InventoryPanel.h"

UInventotySlot_Drag::UInventotySlot_Drag(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (Inventory) {
		if (Inventory->IsValidIndex(Index)) {
			bRotateItem = Inventory->GetItem(Index).IsRotate;
		}
	}
}

bool UInventotySlot_Drag::DropInSlot(UInventoryPanelSlot* Slot, float  SizeSlot) {

	FIntPoint L_CurrentPositin = FIntPoint(Slot->Transform.Offsets.Left / SizeSlot, Slot->Transform.Offsets.Top / SizeSlot);

	if (Slot->ParentPanel->Inventory != Inventory) {
		if (Slot->IndexItem == INDEX_NONE) {
			return Inventory->SendItem(Index, Slot->ParentPanel->Inventory, Count , false, L_CurrentPositin, bRotateItem);
		}
		else {

			auto OtherSlot = Slot->ParentPanel->Inventory->GetItem(Slot->IndexItem);
			auto InvSlot = GetSlot();

			if (InvSlot.GetData().StackItem) {
				if (InvSlot.ItemAsset == OtherSlot.ItemAsset) {
					OtherSlot.Count += Count;

					if (Slot->ParentPanel->Inventory->SetSlot(Slot->IndexItem, OtherSlot)) {
						Inventory->RemoveItem(Index, Count);
						return true;
					}
				}
			}
		}

	}

	if (Slot->ParentPanel->Inventory == Inventory) {
		return Inventory->DropItem(Index, Slot->IndexItem, Count, L_CurrentPositin, Change,false, bRotateItem);
	}

	return false;
}


void UInventotySlot_Drag::SetRotateItem(bool NewRotate)
{
	bRotateItem = NewRotate;
}

void UInventotySlot_Drag::RotateItem()
{
	bRotateItem = !bRotateItem;
}

UInventoryPanelSlot::UInventoryPanelSlot(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	,ParentPanel(nullptr)
	,IndexItem(INDEX_NONE)
	,ZOrder(0)
{
}

void UInventoryPanelSlot::SetMargin(FMargin NewMargin) {
	
	Transform.Offsets = NewMargin;

	if (Slot) 
		Slot->Offset(Transform.Offsets);
}

void UInventoryPanelSlot::ChangeSlot(FInventorySlot NewData) {
	IPanelInventorySlotInterface::Execute_OnChangedSlot(Content, IndexItem, NewData);
}

void UInventoryPanelSlot::BuildSlot(TSharedRef<SConstraintCanvas> GridPanel)
{
	ParentPanel = Cast<UInventoryPanel>(Parent);

	GridPanel->AddSlot()
		.Alignment(FVector2D(0.0f))
		.AutoSize(false)
		.Anchors(FAnchors(0.0f))
		.Expose(Slot)
		[
			Content == nullptr ? SNullWidget::NullWidget : Content->TakeWidget()
		];
}

void UInventoryPanelSlot::SetZOrder(int32 InZOrder)
{
	ZOrder = InZOrder;

	if (Slot)
		Slot->ZOrder(InZOrder);
}

void UInventoryPanelSlot::SetAnchors(FAnchors Anchors)
{
	Transform.Anchors = Anchors;

	if (Slot)
		Slot->Anchors(Transform.Anchors);
}

void UInventoryPanelSlot::SetIndexItem(int32 NewIndex)
{
	if (NewIndex == INDEX_NONE || !Slot) return;

	IndexItem = NewIndex;
	ChangeSlot(ParentPanel->Inventory->GetItem(IndexItem));
}

void UInventoryPanelSlot::SynchronizeProperties() {

	if (IndexItem == INDEX_NONE || !Slot) return;

	ChangeSlot(ParentPanel->Inventory->GetItem(IndexItem));
}