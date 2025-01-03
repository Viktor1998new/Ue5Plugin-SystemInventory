//Copyright(c) 2022 - 2025, Viktor.F.P
#include "InventoryLibrary.h"
#include "InventorySettings.h"

void UInventoryLibrary::SetData(UPARAM(ref) FInventorySlot& Slot, const UScriptStruct* NewValue)
{
	checkNoEntry();
}

bool UInventoryLibrary::GetData(const FInventorySlot Slot, int32& OutStruct)
{
	checkNoEntry();
    return false;
}

FString UInventoryLibrary::StructToJson(UScriptStruct* Struct)
{
	checkNoEntry();
	return FString();
}

bool UInventoryLibrary::JsonToStruct(const FString& Json, int32& OutStruct)
{
	checkNoEntry();
	return false;
}

FIntPoint UInventoryLibrary::GetSlotSize(FInventorySlot Slot)
{
	return Slot.GetSize();
}

FItemData UInventoryLibrary::GetItemData(FInventorySlot Slot)
{
	return Slot.ItemAsset->SlotItemData;
}


UInventorySettings* UInventoryLibrary::GetInventorySetting()
{
	return UInventorySettings::Get();
}