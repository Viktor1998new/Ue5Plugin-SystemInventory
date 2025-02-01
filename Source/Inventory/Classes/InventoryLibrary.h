//Copyright(c) 2022 - 2025, Viktor.F.P
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
    
    UFUNCTION(BlueprintCallable, CustomThunk, meta = (CustomStructureParam = "NewValue"), Category = "InventoryLibrary|Json")
        static void SetData(UPARAM(ref) FInventorySlot& Slot, const  UScriptStruct* NewValue);
    
    DECLARE_FUNCTION(execSetData)
    {
        P_GET_STRUCT_REF(FInventorySlot, Slot);

        Stack.Step(Stack.Object, NULL);
        FStructProperty* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
        void* StructPtr = Stack.MostRecentPropertyAddress;
        P_FINISH;

        P_NATIVE_BEGIN;
        FJsonObjectConverter::UStructToJsonObjectString(StructProperty->Struct, StructPtr, Slot.ItemData);
        P_NATIVE_END;
    }

    UFUNCTION(BlueprintPure, CustomThunk, meta = (CustomStructureParam = "OutStruct"), Category = "InventoryLibrary|Json")
        static bool GetData(const FInventorySlot Slot, int32& OutStruct);

    DECLARE_FUNCTION(execGetData)
    {
        P_GET_STRUCT(FInventorySlot, Slot);
        Stack.MostRecentPropertyAddress = nullptr;
        Stack.StepCompiledIn<FStructProperty>(nullptr);
        void* StructPtr = Stack.MostRecentPropertyAddress;
        FStructProperty* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);

        P_FINISH
            bool bSuccess = false;

        if (!IsValid(Slot.ItemAsset)) {
            bSuccess = false;
            *static_cast<bool*>(RESULT_PARAM) = bSuccess;
            return;
        }

        if ((StructProperty != nullptr) && (StructProperty->Struct != nullptr) && (StructProperty->Struct == Slot.ItemAsset->StructType) && (StructPtr != nullptr))
        {
            TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
            TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Slot.ItemData);
            if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
            {
                bSuccess = false;
                *static_cast<bool*>(RESULT_PARAM) = bSuccess;
                return;
            }
            bSuccess = FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), Slot.ItemAsset->StructType, StructPtr, 0, 0);
        }
        *static_cast<bool*>(RESULT_PARAM) = bSuccess;
    }

    UFUNCTION(BlueprintPure, CustomThunk, meta = (CustomStructureParam = "OutStruct"), Category = "InventoryLibrary|Json")
        static bool JsonToStruct(const FString& Json, int32& OutStruct);

    DECLARE_FUNCTION(execJsonToStruct)
        {
            P_GET_PROPERTY(FStrProperty, Json);

            Stack.MostRecentPropertyAddress = nullptr;
            Stack.StepCompiledIn<FStructProperty>(nullptr);
            void* StructPtr = Stack.MostRecentPropertyAddress;
            FStructProperty* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);

            P_FINISH
                bool bSuccess = false;

            if (!Json.IsEmpty())
            {
                TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
                TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Json);
                if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
                {
                    bSuccess = false;
                    *static_cast<bool*>(RESULT_PARAM) = bSuccess;
                    return;
                }
                bSuccess = FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), StructProperty->Struct, StructPtr, 0, 0);
            }
            *static_cast<bool*>(RESULT_PARAM) = bSuccess;
        }

    UFUNCTION(BlueprintPure, CustomThunk, meta = (CustomStructureParam = "Struct"), Category = "InventoryLibrary|Json")
        static FString StructToJson(UScriptStruct* Struct);

    DECLARE_FUNCTION(execStructToJson)
        {
            Stack.MostRecentPropertyAddress = nullptr;
            Stack.StepCompiledIn<FStructProperty>(nullptr);
            void* StructPtr = Stack.MostRecentPropertyAddress;
            FStructProperty* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);

            P_FINISH

                FString L_Return;

            FJsonObjectConverter::UStructToJsonObjectString(StructProperty->Struct, StructPtr, L_Return, 0, 0);

            *static_cast<FString*>(RESULT_PARAM) = L_Return;
        }

    UFUNCTION(BlueprintPure, Category = "InventoryLibrary")
        static FIntPoint GetSlotSize(FInventorySlot Slot);

    UFUNCTION(BlueprintPure, Category = "InventoryLibrary")
        static FItemData& GetItemData(FInventorySlot Slot);
        
    UFUNCTION(BlueprintPure, Category = "Inventory")
        static UInventorySettings* GetInventorySetting();
};
