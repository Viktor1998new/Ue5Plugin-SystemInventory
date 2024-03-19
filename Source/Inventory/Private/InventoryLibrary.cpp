//Copyright(c) 2022 - 2024, Viktor.F.P
#include "InventoryLibrary.h"
#include "InventorySettings.h"

void UInventoryLibrary::SetData(UPARAM(ref) FInventorySlot& Slot, const UScriptStruct* NewValue)
{
	checkNoEntry();
}

DEFINE_FUNCTION(UInventoryLibrary::execSetData)
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

bool UInventoryLibrary::GetData(const FInventorySlot Slot, int32& OutStruct)
{
	checkNoEntry();
    return false;
}


DEFINE_FUNCTION(UInventoryLibrary::execGetData)
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

FString UInventoryLibrary::GetDataCpp(const FInventorySlot Slot)
{
	if (!IsValid(Slot.ItemAsset) || !IsValid(Slot.ItemAsset->StructType) || Slot.ItemData.IsEmpty()) {
		return FString();
	}

	FString Return_Value;

	if(FJsonObjectConverter::UStructToJsonObjectString(Slot.ItemAsset->StructType->GetSuperStruct(), Slot.ItemAsset->StructType, Return_Value, 0, 0)) {

		return Return_Value;
	}

	return FString();
}

FString UInventoryLibrary::GetJsonString(UScriptStruct* Struct)
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


DEFINE_FUNCTION(UInventoryLibrary::execJsonToStruct)
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


DEFINE_FUNCTION(UInventoryLibrary::execStructToJson)
{
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* StructPtr = Stack.MostRecentPropertyAddress;
	FStructProperty* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);

	P_FINISH

	FString L_Return;

	FJsonObjectConverter::UStructToJsonObjectString(StructProperty->Struct, StructPtr, L_Return, 0, 0);
	* static_cast<FString*>(RESULT_PARAM) = L_Return;
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