//Copyright(c) 2022 - 2024, Viktor.F.P
#include "SlotsWidget.h"
#include "Brushes/SlateColorBrush.h"
#include "PropertyEditorModule.h"
#include "InventoryLibrary.h"

FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

void UMenuContextItemWidget::SetItem(TSharedPtr<class SMenuAnchor> MewMenu, UInventoryComponent* NewInventory, int32 NewIndex)
{
	if(!IsValid(ItemSettings))
		ItemSettings = NewObject<UItemSettings>();

	FInventorySlot L_SlotItem = NewInventory->GetItem(NewIndex);

	ItemSettings->Asset = L_SlotItem.ItemAsset;

	if (UInventoryLibrary::GetDataCpp(L_SlotItem).IsEmpty()) {
		ItemSettings->Data = UInventoryLibrary::GetJsonString(L_SlotItem.ItemAsset->StructType);
	}
	else {
		ItemSettings->Data = L_SlotItem.ItemData;
	}

	ItemSettings->Count = L_SlotItem.Count;

	Inventory = NewInventory;
	Index = NewIndex;
	Menu = MewMenu;
}

FReply UMenuContextItemWidget::OnClickedRemoveItem()
{
	Inventory->RemoveItem(Index, Inventory->GetItem(Index).Count);
	Menu->SetIsOpen(false, false);
	return FReply::Handled();
}

TSharedRef<SWidget> UMenuContextItemWidget::RebuildWidget()
{
	FSlateFontInfo NumderFont = FCoreStyle::GetDefaultFontStyle("Roboto", 9);

	if (!ItemSettings->Asset) {
		ItemSettings->Asset = Inventory->GetItem(Index).ItemAsset;
	}

	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bUpdatesFromSelection = false;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

	TSharedPtr<IDetailsView> MyDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	MyDetailsView->SetObject(ItemSettings);
	MyDetailsView->OnFinishedChangingProperties().AddLambda([this](const FPropertyChangedEvent& Property) {

		if (!Inventory)
			return;

		FInventorySlot L_SlotItem = Inventory->GetItem(Index);
		FInventorySlot L_NewSlotItem = L_SlotItem;
		L_NewSlotItem.ItemAsset = ItemSettings->Asset;
		L_NewSlotItem.ItemData = ItemSettings->Data;

		if(ItemSettings->Asset->SlotItemData.MaxStack < ItemSettings->Count)
			ItemSettings->Count = ItemSettings->Asset->SlotItemData.MaxStack;

		L_NewSlotItem.Count = L_NewSlotItem.ItemAsset->SlotItemData.StackItem ? ItemSettings->Count : 1;

		if (!Inventory->SetSlot(Index, L_NewSlotItem)) {

			ItemSettings->Asset = L_SlotItem.ItemAsset;

			if (UInventoryLibrary::GetDataCpp(L_SlotItem).IsEmpty()) {
				ItemSettings->Data = UInventoryLibrary::GetJsonString(L_SlotItem.ItemAsset->StructType);
			}
			else {
				ItemSettings->Data = L_SlotItem.ItemData;
			}

			return;
		}
	});

	return SNew(SBox)[
		SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)[
				MyDetailsView.ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()[
				SNew(SButton)
					.OnClicked(FOnClicked::CreateUObject(this, &UMenuContextItemWidget::OnClickedRemoveItem))
					.Content()
					[
						SNew(STextBlock)
							.Font(NumderFont)
							.Text(FText::FromString("Remove Item"))
					].HAlign(HAlign_Center).VAlign(VAlign_Center)
			]
	];
}