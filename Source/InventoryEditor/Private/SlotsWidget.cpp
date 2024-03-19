//Copyright(c) 2022 - 2024, Viktor.F.P
#include "SlotsWidget.h"
#include "InventoryUMG/InventoryPanel.h"
#include "Brushes/SlateColorBrush.h"

UInventoryComponent* USlotWidget::GetInventory()
{
	return Cast<UInventoryPanel>(GetParent())->Inventory;
}

void UVisualDragWidget::SetItem(UTexture2D* Texture, FVector2D Size)
{

	if (SizeBox.IsValid()) {
		SizeBox.ToSharedRef()->SetWidthOverride(Size.X);
		SizeBox.ToSharedRef()->SetHeightOverride(Size.Y);
		return;
	}

	FSlateColorBrush* Brush = new  FSlateColorBrush(FLinearColor(0.0f, 0.0f, 0.0f, 0.6f));

	FSlateBrush* BrushTexure = new FSlateBrush();

	if (BrushTexure->GetResourceObject() != Texture)
	{
		BrushTexure->SetResourceObject(Texture);
		BrushTexure->DrawAs = ESlateBrushDrawType::Image;
		BrushTexure->ImageSize = Size;

		if (Texture)
		{
			Texture->bForceMiplevelsToBeResident = true;
			Texture->bIgnoreStreamingMipBias = true;
		}
	}

	SizeBox = SNew(SBox)
		.WidthOverride(Size.X)
		.HeightOverride(Size.Y)[
			SNew(SBorder)
				.BorderImage(Brush)[
					SNew(SImage)
						.Image(BrushTexure)
				]
		];
}

TSharedRef<SWidget> UVisualDragWidget::RebuildWidget()
{
	return SizeBox.ToSharedRef();
}

void UEditor_Drag::RotateItem() {
	IsRotate = !IsRotate;

	if (NewItem) {
		Slot.IsRotate = IsRotate;
		Cast<class UVisualDragWidget>(DefaultDragVisual)->SetItem(Slot.ItemAsset->SlotItemData.ImageItem, Slot.GetSize() * SizeSlot);
	}
	else {
		auto Item_Slot = GetSlot();
		Item_Slot.IsRotate = IsRotate;
		Cast<class UVisualDragWidget>(DefaultDragVisual)->SetItem(Item_Slot.ItemAsset->SlotItemData.ImageItem, Item_Slot.GetSize() * SizeSlot);
	}
}