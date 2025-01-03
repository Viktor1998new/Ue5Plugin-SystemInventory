//Copyright(c) 2022 - 2025, Viktor.F.P
#include "SlotsWidget.h"
#include "Brushes/SlateColorBrush.h"
#include "PropertyEditorModule.h"
#include "InventoryLibrary.h"

FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

FString GetJsonSlot(const FInventorySlot Slot)
{
	if (!IsValid(Slot.ItemAsset) || !IsValid(Slot.ItemAsset->StructType) || Slot.ItemData.IsEmpty()) {
		return FString();
	}

	FString Return_Value;

	if (FJsonObjectConverter::UStructToJsonObjectString(Slot.ItemAsset->StructType->GetSuperStruct(), Slot.ItemAsset->StructType, Return_Value, 0, 0)) {

		return Return_Value;
	}

	return FString();
}

FString GetJsonString(UScriptStruct* Struct)
{
	if (!IsValid(Struct))
		return FString();

	uint8* DefaultStructInstance = (uint8*)FMemory::Malloc(Struct->GetStructureSize());
	Struct->InitializeDefaultValue(DefaultStructInstance);

	FString L_Return;

	if (FJsonObjectConverter::UStructToJsonObjectString(Struct, DefaultStructInstance, L_Return, 0, 0)) {

		FMemory::Free(DefaultStructInstance);
		return L_Return;
	}

	FMemory::Free(DefaultStructInstance);
	return FString();

}

void UMenuContextItemWidget::CheckSettingsData(FInventorySlot InventorySlot) {

	if (GetJsonSlot(InventorySlot).IsEmpty()) {
		ItemSettings->Data = GetJsonString(InventorySlot.ItemAsset->StructType);

	}
	else {
		ItemSettings->Data = InventorySlot.ItemData;
	}
}

void UMenuContextItemWidget::SetItem(TSharedPtr<class SMenuAnchor> MewMenu, UInventoryComponent* NewInventory, int32 NewIndex)
{
	if(!IsValid(ItemSettings))
		ItemSettings = NewObject<UItemSettings>();

	FInventorySlot L_SlotItem = NewInventory->GetItem(NewIndex);
	ItemSettings->Asset = L_SlotItem.ItemAsset;
	
	CheckSettingsData(L_SlotItem);

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
			
			CheckSettingsData(L_SlotItem);

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