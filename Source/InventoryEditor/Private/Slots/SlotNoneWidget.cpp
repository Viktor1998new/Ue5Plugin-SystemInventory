//Copyright(c) 2022 - 2025, Viktor.F.P
#include "SlotsWidget.h"
#include "InventoryUMG/InventoryPanelSlot.h"
#include "Brushes/SlateColorBrush.h"

bool USlotNoneWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UInventoryPanelSlot* L_Slot = Cast<UInventoryPanelSlot>(Slot);

	if (!IsValid(L_Slot))
		return false;

	if (auto L_Operation = Cast<UInventotySlot_Drag>(InOperation)) {

		if (auto SlotAsset = Cast<USlotAssetWidget>(L_Operation->Payload))
		{
			FIntPoint L_CurrentPositin = FIntPoint(L_Slot->Transform.Offsets.Left / SizeSlot, L_Slot->Transform.Offsets.Top / SizeSlot);
			UInventoryComponent* L_Inventory = GetInventory();
			FInventorySlot L_NewSlot;
			L_NewSlot.ItemAsset = SlotAsset->Asset;
			L_NewSlot.PositionSlot = L_CurrentPositin;
			L_NewSlot.Count = 1;
			L_NewSlot.IsRotate = L_Operation->GetRotateItem();

			int32 L_Index;
			return	L_Inventory->AddSlot(L_NewSlot, false, L_Index);
		}

		return L_Operation->DropInSlot(L_Slot, SizeSlot);
	}

	return false;
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