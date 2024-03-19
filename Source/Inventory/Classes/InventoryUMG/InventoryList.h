//Copyright(c) 2022 - 2024, Viktor.F.P

#pragma once

#include "InventoryPanel.h"
#include "InventoryList.generated.h"

UCLASS()
class INVENTORY_API UInventoryList : public UInventoryPanel
{
	GENERATED_UCLASS_BODY()


public:

	UPROPERTY(EditAnywhere, NoClear, Category = "InventoryList")
		TSubclassOf<UUserWidget> ItemSlot;

	UPROPERTY(EditAnywhere, Category = "InventoryList")
		TSubclassOf<UUserWidget> NoneSlot;

	UPROPERTY(EditAnywhere, Category = "InventoryList")
		float SizeSlot = 32.0f;

	virtual void SetInventory(UInventoryComponent* NewInventory) override;

protected:

	void AddSlot(int32 IndexItem);

	virtual	void OnChangeSlot(int32 Index, FInventorySlot NewData, ETypeSetItem Type) override;

	void ChangeSlots(int32 Index, FInventorySlot NewData, ETypeSetItem Type);

	void SetSlotPosition(UInventoryPanelSlot* ChangeSlot, float Position);
};
