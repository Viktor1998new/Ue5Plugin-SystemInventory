//Copyright(c) 2022 - 2024, Viktor.F.P

#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PanelInventorySlotInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPanelInventorySlotInterface : public UInterface
{
	GENERATED_BODY()
};

class INVENTORY_API IPanelInventorySlotInterface
{
	GENERATED_IINTERFACE_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category = "InventoryPanel|Slot")
	void OnChangedSlot(int32 NewIndex, FInventorySlot NewSlot);
};
