//Copyright(c) 2022 - 2024, Viktor.F.P

#include "InventoryUMG/InventoryPanel_Blueprint.h"
#include "InventoryUMG/InventoryPanelSlot.h"

#define LOCTEXT_NAMESPACE "UMG"

UInventoryPanel_Blueprint::UInventoryPanel_Blueprint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UInventoryPanel_Blueprint::SetInventory(UInventoryComponent* NewInventory)
{
	Super::SetInventory(NewInventory);

	ReceiveInitInventory(NewInventory);
}

void UInventoryPanel_Blueprint::OnChangeSlot(int32 Index, FInventorySlot NewData, ETypeSetItem Type)
{
	ReceiveOnChangeSlot(Index, NewData, Type);
}