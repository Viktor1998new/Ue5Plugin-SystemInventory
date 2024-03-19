//Copyright(c) 2022 - 2024, Viktor.F.P
#include "SlotsWidget.h"
#include "InventoryUMG/InventoryPanelSlot.h"
#include "InventoryUMG/InventoryPanel.h"
#include "Brushes/SlateColorBrush.h"
#include "InventorySettings.h"

void USlotAssetWidget::SetAssetItem(UItemAsset* NewAsset)
{
	if (!NewAsset)
		return;

	Asset = NewAsset;
	UTexture2D* Texture = Asset->SlotItemData.ImageItem;

	FSlateBrush* Brush = new FSlateBrush();

	if (Brush->GetResourceObject() != Texture)
	{
		Brush->SetResourceObject(Texture);

		if (Texture)
		{
			Texture->bForceMiplevelsToBeResident = true;
			Texture->bIgnoreStreamingMipBias = true;
			Brush->ImageSize = FVector2D(64.f, 64.f);
		}

		ItemIcon.ToSharedRef()->SetImage(Brush);
	}

	NameAsset.ToSharedRef()->SetText(FText::FromString(Asset->GetName()));
}

FReply USlotAssetWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton)) {

		SetKeyboardFocus();

		TSharedPtr<SWidget> SlateWidgetDetectingDrag = GetCachedWidget();

		if (SlateWidgetDetectingDrag.IsValid())
		{
			return FReply::Handled().DetectDrag(SlateWidgetDetectingDrag.ToSharedRef(), EKeys::LeftMouseButton);
		}
	}
	return FReply::Handled();
}

void USlotAssetWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	operation = NewObject<UEditor_Drag>();

	operation->NewItem = true;
	operation->Pivot = EDragPivot::TopLeft;

	auto DragVisual = CreateWidget<UVisualDragWidget>(this);

	FIntPoint Size = Asset->SlotItemData.SizeSlot;

	if (!UInventorySettings::Get()->HasInventoryFlag(EInventoryFlag::Size))
		Size = FIntPoint(1);

	DragVisual->SetItem(Asset->SlotItemData.ImageItem, Size * SizeSlot);

	operation->DefaultDragVisual = DragVisual;

	FInventorySlot NewSlot;
	NewSlot.ItemAsset = Asset;
	NewSlot.Count = 1;

	operation->Slot = NewSlot;

	OutOperation = operation;
}

FReply USlotAssetWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftShift) {
		if (IsValid(operation)) {
			operation->RotateItem();
		}
	}
	return FReply::Handled();
}

TSharedRef<SWidget> USlotAssetWidget::RebuildWidget() {
	bIsFocusable = true;

	FSlateColorBrush* Brush = new  FSlateColorBrush(FLinearColor(0.0f, 0.0f, 0.0f, 0.6f));
	FSlateFontInfo NumderFont = FCoreStyle::GetDefaultFontStyle("Roboto", 9);

	return SNew(SBox)
		.WidthOverride(128.0f)
		.HeightOverride(128.0f)
		.MaxDesiredHeight(128.0f)
		.MaxDesiredWidth(128.0f)
		[
			SNew(SBorder)
				.BorderImage(Brush)[
					SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.Padding(FMargin(10.f))[
							SAssignNew(ItemIcon, SImage)
						]

						+ SVerticalBox::Slot()
						.AutoHeight()[
							SNew(SSpacer)
								.Size(FVector2D(1.f, 5.f))
						]

						+ SVerticalBox::Slot()
						.FillHeight(1.f)[
							SAssignNew(NameAsset, STextBlock)
								.Font(NumderFont)
								.Text(FText::FromString(""))
								.Justification(ETextJustify::Center)
								.AutoWrapText(true)
								.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
								.Clipping(EWidgetClipping::ClipToBounds)

						]
				]
		];
}
