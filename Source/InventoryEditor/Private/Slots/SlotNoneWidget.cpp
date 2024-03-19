//Copyright(c) 2022 - 2024, Viktor.F.P
#include "SlotsWidget.h"
#include "InventoryUMG/InventoryPanelSlot.h"
#include "Brushes/SlateColorBrush.h"

bool USlotNoneWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UInventoryPanelSlot* L_Slot = Cast<UInventoryPanelSlot>(Slot);

	if (!IsValid(L_Slot))
		return false;

	FIntPoint L_CurrentPositin = FIntPoint(L_Slot->Transform.Offsets.Left / SizeSlot, L_Slot->Transform.Offsets.Top / SizeSlot);
	UInventoryComponent* L_Inventory = GetInventory();

	if (auto Operation = Cast<UEditor_Drag>(InOperation)) {

		if (Operation->NewItem) {
			FInventorySlot LNewSlot = Operation->Slot;
			LNewSlot.PositionSlot = L_CurrentPositin;
			int32 L_Index;
			L_Inventory->AddSlot(LNewSlot, false, L_Index);
		}
		else {
			if (Operation->Inventory == L_Inventory) {
				bool OldRotation = Operation->Inventory->GetItem(Operation->Index).IsRotate;
				auto* L_SlotItem = &Operation->Inventory->GetItem(Operation->Index);
				L_SlotItem->IsRotate = Operation->IsRotate;
				if (Operation->Inventory->DropItem(Operation->Index, -1, 1, L_CurrentPositin, true, true)) {
					if (L_SlotItem->Count != 1) {
						L_SlotItem->IsRotate = OldRotation;
					}
					Operation->Inventory->NewDataSlot.Broadcast(Operation->Index, Operation->GetSlot(), ETypeSetItem::ChangeSlot);
				}
				else {
					L_SlotItem->IsRotate = OldRotation;
				}
				Cast<USlotItemWidget>(Operation->Payload)->SetVisible();
				return true;
			}

			Operation->Inventory->GetItem(Operation->Index).IsRotate = Operation->IsRotate;
			Operation->Inventory->SendItem(Operation->Index, L_Inventory, 1, false, L_CurrentPositin);
			Cast<USlotItemWidget>(Operation->Payload)->SetVisible();
			return true;
		}
	}

	return true;
}

TSharedRef<SWidget> USlotNoneWidget::RebuildWidget()
{
	FSlateColorBrush* BrushButtons = new  FSlateColorBrush(FLinearColor(0.0f, 0.0f, 0.0f));

	MyPanel = SNew(SConstraintCanvas)
		+ SConstraintCanvas::Slot()
		.Anchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f))
		.Offset(FMargin(1.5f, 1.5f, 1.5f, 1.5f))
		[
			SNew(SBorder)
				.BorderImage(BrushButtons)
		];

	return  MyPanel.ToSharedRef();
}

FMargin USlotItemWidget::GetOffsetMouse() const
{
	return FMargin(MousePosition.X, MousePosition.Y);
}

void USlotItemWidget::SetVisible()
{
	SetVisibility(ESlateVisibility::Visible);
	ImageItem->SetVisibility(TAttribute<EVisibility>(EVisibility::Visible));
}