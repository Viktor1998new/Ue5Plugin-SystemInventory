//Copyright(c) 2022 - 2024, Viktor.F.P
#pragma once
#include "CoreMinimal.h"
#include "UObject/Script.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Inventory/Classes/Inventory/InventoryComponent.h"
#include "InventoryLibrary.generated.h"

UCLASS()
class INVENTORY_API UInventoryLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    
    UFUNCTION(BlueprintCallable, CustomThunk, meta = (CustomStructureParam = "NewValue"), Category = "InventoryLibrary")
        static void SetData(UPARAM(ref) FInventorySlot& Slot, const  UScriptStruct* NewValue);
    
    DECLARE_FUNCTION(execSetData);

    UFUNCTION(BlueprintPure, CustomThunk, meta = (CustomStructureParam = "OutStruct"))
        static bool GetData(const FInventorySlot StructType, int32& OutStruct);

    DECLARE_FUNCTION(execGetData);

    static FString GetDataCpp(const FInventorySlot Slot);

    static FString GetJsonString(UScriptStruct* Struct);

    UFUNCTION(BlueprintPure, CustomThunk, meta = (CustomStructureParam = "Struct"))
        static FString StructToJson(UScriptStruct* Struct);
    
    DECLARE_FUNCTION(execStructToJson);

    UFUNCTION(BlueprintPure, CustomThunk, meta = (CustomStructureParam = "OutStruct"))
        static bool JsonToStruct(const FString& Json, int32& OutStruct);

    DECLARE_FUNCTION(execJsonToStruct);

    UFUNCTION(BlueprintPure, Category = "InventoryLibrary")
        static FIntPoint GetSlotSize(FInventorySlot Slot);

    UFUNCTION(BlueprintPure, Category = "InventoryLibrary")
        static FItemData GetItemData(FInventorySlot Slot);
        
    UFUNCTION(BlueprintPure, Category = "Inventory")
        static UInventorySettings* GetInventorySetting();
};
