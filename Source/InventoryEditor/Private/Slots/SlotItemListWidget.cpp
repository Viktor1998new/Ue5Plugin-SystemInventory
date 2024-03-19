//Copyright(c) 2022 - 2024, Viktor.F.P
#include "SlotsWidget.h"
#include "InventoryUMG/InventoryPanel.h"
#include "InventoryUMG/InventoryPanelSlot.h"
#include "Brushes/SlateColorBrush.h"

FMargin USlotItemListWidget::GetOffsetMouse() const
{
	return FMargin(MousePosition.X, MousePosition.Y);
}

FReply USlotItemListWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{

	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton)) {

		MousePosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		MenuAnchor->SetIsOpen(true, true);
		ContextMenu->SetItem(MenuAnchor, Cast<UInventoryPanel>(GetParent())->Inventory, Item_Index);
	}

	return FReply::Handled();
}

void USlotItemListWidget::OnChangedSlot_Implementation(int32 NewIndex, FInventorySlot NewSlot)
{
	Item_Index = NewIndex;

	if (!NewSlot.ItemAsset)
		return;

	UItemAsset* Asset = NewSlot.ItemAsset;
	UTexture2D* Texture = Asset->SlotItemData.ImageItem;

	FSlateBrush* Brush = new FSlateBrush();

	if (Brush->GetResourceObject() != Texture)
	{
		Brush->SetResourceObject(Texture);

		if (Texture)
		{
			Texture->bForceMiplevelsToBeResident = true;
			Texture->bIgnoreStreamingMipBias = true;
			Brush->ImageSize = FVector2D(32.f, 32.f);
		}
		if (Asset->SlotItemData.NameItem.IsEmpty())
			Text_Name.ToSharedRef()->SetText(FText::FromString("No Name"));
		else
			Text_Name.ToSharedRef()->SetText(Asset->SlotItemData.NameItem);

		Image.ToSharedRef()->SetImage(Brush);
	}

}

TSharedRef<SWidget> USlotItemListWidget::HandleGetMenuContent()
{
	UInventoryPanel* Panel = Cast<UInventoryPanel>(GetParent());

	ContextMenu = CreateWidget<UMenuContextItemWidget>(this);

	return ContextMenu->TakeWidget();
}

TSharedRef<SWidget> USlotItemListWidget::RebuildWidget()
{
	FSlateColorBrush* BrushSlot = new  FSlateColorBrush(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f));
	FSlateFontInfo FontName = FCoreStyle::GetDefaultFontStyle("Roboto", 12);
	FSlateFontInfo FontData = FCoreStyle::GetDefaultFontStyle("Roboto", 10);

	TSharedPtr<SHorizontalBox> Horizontal = SNew(SHorizontalBox);

	Horizontal->AddSlot()
		.AutoWidth()
		.Padding(FMargin(10.0f, 0.0f, 0.0f, 0.0f))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)[
			SAssignNew(Image, SImage)
		];

	Horizontal->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Center)[
			SNew(SSpacer)
				.Size(FVector2D(20.0f, 0.0f))
		];

	Horizontal->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)[

			SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 5.0f, 0.0f, 0.0f))
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Fill)[
					SAssignNew(Text_Name, STextBlock)
						.Font(FontName)
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 10.0f, 0.0f, 0.0f))
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Fill)[
					SAssignNew(Text_Data, STextBlock)
						.Font(FontData)
				]
		];

	TSharedPtr<SConstraintCanvas> Panel = SNew(SConstraintCanvas)
		+ SConstraintCanvas::Slot()
		.AutoSize(true)
		.Anchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f))
		.Offset(FMargin(0.0f, 0.0f, 0.0f, 0.0f))[
			SNew(SBorder)
				.BorderImage(BrushSlot)[
					Horizontal.ToSharedRef()
				]
		];

	A_Offset.BindUObject(this, &USlotItemListWidget::GetOffsetMouse);

	Panel.ToSharedRef()->AddSlot()
		.Offset(A_Offset)
		.AutoSize(true)[
			SAssignNew(MenuAnchor, SMenuAnchor)
				.Placement(EMenuPlacement::MenuPlacement_ComboBox)
				.OnGetMenuContent(FOnGetContent::CreateUObject(this, &USlotItemListWidget::HandleGetMenuContent))];

	return Panel.ToSharedRef();
}