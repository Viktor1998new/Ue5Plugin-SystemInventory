//Copyright(c) 2022 - 2024, Viktor.F.P

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemActor.h"
#include "InventoryComponent.generated.h"

class UInventorySettings;

USTRUCT(BlueprintType)
struct INVENTORY_API FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Slot")
		UItemAsset* ItemAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Slot")
		FIntPoint PositionSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Slot")
		FString ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Slot")
		int32 Count;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Slot")
		bool IsRotate = false;

	bool IsPosition(FIntPoint Position, FIntPoint Size);

	bool operator == (const FInventorySlot& A) const {

		return ItemAsset == A.ItemAsset && ItemData == A.ItemData;
	}

	void operator ++() {
		Count++;
	}

	void operator --() {
		Count--;
	}

	FIntPoint GetSize();
};

USTRUCT(BlueprintType)
struct INVENTORY_API FInventory
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly, Category = "Inventory")
		TArray<FInventorySlot> Items;

	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly, Category = "Inventory")
		float Massa;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Inventory")
		int32 CountRow;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Inventory")
		int32 MaxSlot;

	FInventory();
};

UENUM(BlueprintType)
enum class ETypeSetItem : uint8
{
	Add,
	Remove,
	ChangeSlot,
	SetPosition
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddItem, int32, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemoveItem, int32, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FNewDataSlot, int32, Index, FInventorySlot, NewData, ETypeSetItem, SetType);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), NotBlueprintable, BlueprintType)
class INVENTORY_API UInventoryComponent : public UActorComponent {
	
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Inventory")
		FInventory Inventory;

	UPROPERTY(BlueprintAssignable)
		FOnAddItem OnAddItem;

	UPROPERTY(BlueprintAssignable)
		FOnRemoveItem OnRemoveItem;

	UPROPERTY(BlueprintAssignable)
		FNewDataSlot NewDataSlot;

	// Sets default values for this component's properties
	UInventoryComponent();

public:

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetInventory(FInventory NewInventory) {
		Inventory = NewInventory;
	};


	UFUNCTION(BlueprintPure, Category = "Inventory")
	FInventory& GetInventory() {
		return Inventory;
	};

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FInventorySlot& GetItem(int32 Index) {
		return Inventory.Items[Index];
	};
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 CountItems() const {
		return Inventory.Items.Num();
	};

	/*Adding a slot
	 * @param NewSlot - The new active state of the component
	  Use only in Server*/
	UFUNCTION(BlueprintCallable, Category = "Inventory|Slot|Add")
		virtual	bool AddSlot(FInventorySlot NewSlot, bool FindPositionSlot, int32& Index);

	/*Adding an item
	 Use only in Server*/
	UFUNCTION(BlueprintCallable, Category = "Inventory|Slot|Add")
		bool AddActorItem(AItemActor* Item, int32& Index);

	/*Adding an item
	 Use only in Server*/
	UFUNCTION(BlueprintCallable, Category = "Inventory|Slot")
		virtual bool SetSlot(int32 Index, FInventorySlot NewValue);

	/*Adding an item
	 Use only in Server*/
	UFUNCTION(BlueprintCallable, Category = "Inventory|Slot|Add")
		bool AddAssetItem(UItemAsset* ItemAsset, int32 Count, const FString& Data, int32& Index);

	/*The function for searching for free space and can also be used to check whether the slot fits into the inventory */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool FindFreeSlot(FIntPoint Size, FIntPoint& ReturnPosition);

	/*search function for an item by class or class child*/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool FindItem(UItemAsset* ItemAsset, int32& Index);

	/*Removing an item
	 Use only in Server*/
	UFUNCTION(BlueprintCallable, Category = "Inventory|Slot")
		bool RemoveItem(int32 Index, int32 count);

	/*the function sends an item from the current inventory to another one
	 Use only in Server*/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool SendItem(int32 Index, class UInventoryComponent* ToIntentory, int32 Count, bool FindSlot, FIntPoint Position);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetMaxSlot(int32 NewValue) {
			Inventory.MaxSlot = NewValue;
		}

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetCountRow(int32 NewValue) {
			Inventory.CountRow = NewValue;
		}

	/*the function checks whether the element can be placed in this position*/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool IsPositionFree(FIntPoint Position, FIntPoint Size, int32& Index);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetRotateSlot(int32 Index, bool NewRotate);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Slot")
		bool DropItem(int32 IndexItem, int32 ToIndex, int32 Count, FIntPoint ToPosition, bool Change, bool FindPosition);

	UFUNCTION(Client, Reliable)
		void ClientRPC_EventSetItem(int32 Index, FInventorySlot NewData, ETypeSetItem Type);

#if WITH_EDITOR
	// Only Edit
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void RecalculationMass();
#endif

protected:
	void ChangeSlot(int32 Index, FInventorySlot Slot, ETypeSetItem Type);

private:
	int GetCountRow(bool IsEnd) const;

};


UCLASS(Abstract, ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class INVENTORY_API UInventoryComponent_Blueprintable : public UInventoryComponent {

	GENERATED_BODY()
public:

	virtual bool SetSlot(int32 Index, FInventorySlot NewValue) override {

		float DelMass = 0, AddMass = 0;
		if (Inventory.Items[Index].ItemAsset != NewValue.ItemAsset || Inventory.Items[Index].Count != NewValue.Count) {
			DelMass = Inventory.Items[Index].ItemAsset->SlotItemData.MassItem * Inventory.Items[Index].Count;
			AddMass = NewValue.ItemAsset->SlotItemData.MassItem * NewValue.Count;
		}

		bool Is_Seted = ReceiveSetSlot(Index, NewValue);

		if (Is_Seted) {
			Inventory.Massa -= DelMass;
			Inventory.Massa += AddMass;
			ChangeSlot(Index, NewValue, ETypeSetItem::ChangeSlot);
		}

		return Is_Seted;
	};

	virtual	bool AddSlot(FInventorySlot NewSlot, bool FindPositionSlot, int32& Index) override {

		bool L_IsAdding = ReceiveAddSlot(NewSlot, FindPositionSlot, Index);

		if (L_IsAdding) {
			OnAddItem.Broadcast(Index);
			Inventory.Massa += NewSlot.ItemAsset->SlotItemData.MassItem * NewSlot.Count;
			ChangeSlot(Index, NewSlot, ETypeSetItem::Add);
		}

		return L_IsAdding;
	};

protected:
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Add Slot"))
		bool ReceiveAddSlot(FInventorySlot NewSlot, bool FindPositionSlot, int32& Index);
		
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Set Slot"))
		bool ReceiveSetSlot(int32 Index, FInventorySlot NewValue);
};