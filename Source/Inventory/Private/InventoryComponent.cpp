//Copyright(c) 2022 - 2025, Viktor.F.P

#include "Inventory/InventoryComponent.h"
#include "InventorySettings.h"
#include "Net/UnrealNetwork.h"

#define MaXCountRow FMath::CeilToInt(Inventory.MaxSlot / Inventory.CountRow) + 1

#define HasInventoryFlag(Flag) UInventorySettings::Get()->HasInventoryFlag(Flag)

void UInventoryComponent::ChangeSlot(int32 Index, FInventorySlot Slot, ETypeSetItem Type) {

	if(GIsServer)
		ClientRPC_EventSetItem(Index, Slot, Type);
	else
		NewDataSlot.Broadcast(Index, Slot, Type);
}

FInventory::FInventory() {
	Massa = 0.0f;
	MaxSlot = 0;

};

bool FInventorySlot::IsPosition(FIntPoint Position, FIntPoint Size) {
	
	if (HasInventoryFlag(EInventoryFlag::Size)) {
		
		FIntPoint SizeItem = GetSize();
		
		SizeItem -= 1;
		Size -= 1;

		if (Position.X >= PositionSlot.X ? Position.X <= PositionSlot.X + SizeItem.X : Position.X + Size.X >= PositionSlot.X) {
			
			if (HasInventoryFlag(EInventoryFlag::OnlyX)) {
				return true;
			}

			if (Position.Y >= PositionSlot.Y ? Position.Y <= PositionSlot.Y + SizeItem.Y : Position.Y + Size.Y >= PositionSlot.Y) {
				return true;
			}
		}
	}
	else {
		if (Position == PositionSlot) {
			return true;
		}
	}

	return false;
}

FItemData& FInventorySlot::GetData() {
	return ItemAsset->SlotItemData;
}

FIntPoint FInventorySlot::GetSize()
{
	if (!HasInventoryFlag(EInventoryFlag::Size)) {
		return FIntPoint(1);
	}

	if (!IsValid(ItemAsset))
		return FIntPoint(0);

	FIntPoint L_ReturnValue = GetData().SizeSlot;

	if (IsRotate)
		return FIntPoint(L_ReturnValue.Y, L_ReturnValue.X);

	return L_ReturnValue;
}

FIntPoint FInventorySlot::GetSize(bool NewRotate)
{
	if (!HasInventoryFlag(EInventoryFlag::Size)) {
		return FIntPoint(1);
	}

	if (!IsValid(ItemAsset))
		return FIntPoint(0);

	FIntPoint L_ReturnValue = GetData().SizeSlot;

	if (NewRotate)
		return FIntPoint(L_ReturnValue.Y, L_ReturnValue.X);

	return L_ReturnValue;
}

int UInventoryComponent::GetCountRow(bool IsEnd) const {

	if (IsEnd)
		return Inventory.MaxSlot % Inventory.CountRow;

	return Inventory.CountRow;
}


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UInventoryComponent, Inventory, COND_OwnerOnly);
}

void UInventoryComponent::ClientRPC_EventSetItem_Implementation(int32 Index, FInventorySlot NewData, ETypeSetItem Type)
{
	NewDataSlot.Broadcast(Index, NewData, Type);
}

bool UInventoryComponent::SetSlot(int32 Index, FInventorySlot NewValue) {

	if (!NewValue.ItemAsset || !Inventory.Items.IsValidIndex(Index) || NewValue.Count <= 0 || NewValue.Count > NewValue.GetData().MaxStack)
		return false;

	if (Inventory.Items[Index].ItemAsset != NewValue.ItemAsset || Inventory.Items[Index].PositionSlot != NewValue.PositionSlot)
	{
		for (int32 i = 0; i < Inventory.Items.Num(); i++)
		{
			if (NewValue.GetData().StackItem)
				if (i == Index)
					continue;

			if (Inventory.Items[i].ItemAsset) {
				if (Inventory.Items[i].IsPosition(NewValue.PositionSlot, NewValue.GetSize())) {
					
					return false;
				}
			}
		}
	}
	
	if (HasInventoryFlag(EInventoryFlag::Mass)) {
		if (Inventory.Items[Index].ItemAsset != NewValue.ItemAsset || Inventory.Items[Index].Count != NewValue.Count) {
			Inventory.Massa -= Inventory.Items[Index].GetData().MassItem * Inventory.Items[Index].Count;
			Inventory.Massa += NewValue.GetData().MassItem * NewValue.Count;
		}
	}
	
	Inventory.Items[Index] = NewValue;

	ChangeSlot(Index, NewValue, ETypeSetItem::ChangeSlot);

	return true;
}

bool UInventoryComponent::AddSlot(FInventorySlot NewSlot, bool FindPositionSlot, int32& Index) {

	if (!NewSlot.ItemAsset || NewSlot.Count <= 0) {
		return false;
	}

	int32 IndexFull;

	if (!IsPositionFree(NewSlot.PositionSlot, NewSlot.GetSize(), IndexFull)) {

		if (FindPositionSlot) {
			FIntPoint NewPosition;
			bool IsFindPos = FindFreeSlot(NewSlot.GetSize(), NewPosition);

			if (IsFindPos) {
				NewSlot.PositionSlot = NewPosition;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}

	if (!NewSlot.GetData().StackItem && NewSlot.Count > NewSlot.GetData().MaxStack) {
		return false;
	}
	else NewSlot.Count = 1;

	if (HasInventoryFlag(EInventoryFlag::Mass)) {
		Inventory.Massa += NewSlot.GetData().MassItem * NewSlot.Count;
	}

	int32 AddIndex = Inventory.Items.Add(NewSlot);
	Index = AddIndex;

	OnAddItem.Broadcast(AddIndex);
	ChangeSlot(AddIndex, NewSlot, ETypeSetItem::Add);

	return true;
}

bool UInventoryComponent::AddActorItem(AItemActor* Item, int32& Index)
{
	return AddAssetItem(Item->ItemAsset, 1, Item->GetData(), Index);
}

bool UInventoryComponent::AddAssetItem(UItemAsset* ItemAsset, int32 Count, const FString& Data, int32& Index)
{
	if (!ItemAsset || Count <= 0 || Count > ItemAsset->SlotItemData.MaxStack) return false;

	if (ItemAsset->SlotItemData.StackItem) {
		
		int32 FindIndex = INDEX_NONE;

		if (!IsValid(ItemAsset->StructType)) {
			for (int32 i = 0; i < Inventory.Items.Num(); i++) {
				if (Inventory.Items[i].ItemAsset == ItemAsset){
					FindIndex = i;
					break;
				}
			}
		}
		else {
			for (int32 i = 0; i < Inventory.Items.Num(); i++) {
				if (Inventory.Items[i].ItemAsset == ItemAsset && Inventory.Items[i].ItemData == Data) {
					FindIndex = i;
					break;
				}
			}
		}

		if (FindIndex != INDEX_NONE) {

			auto L_ChangeSlot = Inventory.Items[FindIndex];
				
			L_ChangeSlot.Count += Count;
			
			bool L_IsAdd = SetSlot(FindIndex, L_ChangeSlot);
			
			if(L_IsAdd)
				OnAddItem.Broadcast(FindIndex);
			
			return true;
		}
	}

	FInventorySlot NewSlot;
	NewSlot.ItemAsset = ItemAsset;
	NewSlot.ItemData = Data;
	NewSlot.Count = Count;
	
	return AddSlot(NewSlot, true, Index);
}

bool UInventoryComponent::FindFreeSlot(FIntPoint Size, FIntPoint& ReturnPosition)
{
	// Check restrictions on Y
	bool LimitY = !HasInventoryFlag(EInventoryFlag::OnlyX) ?
		(HasInventoryFlag(EInventoryFlag::LimitY) ? Size.Y <= MaXCountRow : true) : true;

	// Check for exceeding inventory limits
	if (Size.X > Inventory.CountRow || !LimitY) {
		return false;
	}

	// If inventory is empty, return the starting position
	if (Inventory.Items.Num() == 0) {
		ReturnPosition = FIntPoint::ZeroValue;
		return true;
	}

	FIntPoint NewPosition(0, 0);
	bool IsNoFree;
	int32 L_ItemIndex;

	// Cache flags
	int32 L_MaXCountRow = MaXCountRow;
	bool L_IsSizeItem = HasInventoryFlag(EInventoryFlag::Size);
	bool L_IsOnlyX = HasInventoryFlag(EInventoryFlag::OnlyX);
	bool L_IsLimitY = HasInventoryFlag(EInventoryFlag::LimitY);

	/* the cycle will end if IsNoFree == true (an empty space is found)
	or Y reaches a linite, 
	if there is no limit it will continue until it finds a free space */

	do {
		if (L_IsOnlyX) {
			if (NewPosition.X + (Size.X - 1) > (GetCountRow(Size.Y + NewPosition.Y == L_MaXCountRow) - 1)) {
				return false;
			}
		}
		else {
			if (NewPosition.X + (Size.X - 1) > (GetCountRow(Size.Y + NewPosition.Y == L_MaXCountRow) - 1)) {
				NewPosition.X = 0;

				if (L_IsLimitY)
					if (NewPosition.Y + (Size.Y - 1) > (L_MaXCountRow - 1))
						return false;

				NewPosition.Y++;
			}
		}

		IsNoFree = !IsPositionFree(NewPosition, Size, L_ItemIndex);

		if (IsNoFree) {

			if (L_IsSizeItem && Inventory.Items.IsValidIndex(L_ItemIndex))
				NewPosition.X = Inventory.Items[L_ItemIndex].PositionSlot.X + Inventory.Items[L_ItemIndex].GetSize().X;
			else 
				NewPosition.X++;
			
		}

	} while (IsNoFree);

	ReturnPosition = NewPosition;

	return true;
}

bool UInventoryComponent::FindItem(UItemAsset* ItemAsset, int32& Index)
{
	if (!IsValid(ItemAsset)) {
		Index = INDEX_NONE;
		return false;
	}

	for (int32 i = 0; i < Inventory.Items.Num(); i++)
		if (Inventory.Items[i].ItemAsset == ItemAsset) {
			Index = i;
			return true;
		}

	Index = INDEX_NONE;
	return false;
}

bool UInventoryComponent::RemoveItem(int32 Index, int32 Count) {

	if (!Inventory.Items.IsValidIndex(Index))
		return false;

	if (Inventory.Items[Index].GetData().StackItem) {

		if (Inventory.Items[Index].Count >= Count) {

			if (HasInventoryFlag(EInventoryFlag::Mass))
				Inventory.Massa -= Inventory.Items[Index].GetData().MassItem * Count;

			if (Inventory.Items[Index].Count - Count == 0) {
				
				OnRemoveItem.Broadcast(Index);
				Inventory.Items.RemoveAt(Index);
				ChangeSlot(Index, FInventorySlot(), ETypeSetItem::Remove);
				
			}
			else {
				OnRemoveItem.Broadcast(Index);
				Inventory.Items[Index].Count -= Count;
				ChangeSlot(Index, Inventory.Items[Index], ETypeSetItem::ChangeSlot);
			}

			return true;
		}
	}

	if (HasInventoryFlag(EInventoryFlag::Mass))
		Inventory.Massa -= Inventory.Items[Index].GetData().MassItem;

	OnRemoveItem.Broadcast(Index);
	Inventory.Items.RemoveAt(Index);
	ChangeSlot(Index, FInventorySlot(), ETypeSetItem::Remove);

	return true;
}

bool UInventoryComponent::SendItem(int32 Index, UInventoryComponent* ToIntentory, int32 Count, bool FindSlot, FIntPoint Position, bool Rotate)
{
	if(!ToIntentory|| !Inventory.Items.IsValidIndex(Index) || this == ToIntentory || Inventory.Items[Index].Count < Count) return false;

	FInventorySlot NewStot = Inventory.Items[Index];
	NewStot.Count = Count;
	NewStot.IsRotate = Rotate;
	NewStot.PositionSlot = Position;
	
	int32 AddIndex;
	
	if (ToIntentory->AddSlot(NewStot, FindSlot, AddIndex)) {
		
		RemoveItem(Index, Count);
		return true;
	}

	return false;
}

bool UInventoryComponent::IsPositionFree(FIntPoint Position, FIntPoint Size, int32& Index)
{

	if (Position.X + Size.X > GetCountRow(Position.Y + Size.Y == MaXCountRow))
	{
		Index = INDEX_NONE;
		return false;
	}

	if(!HasInventoryFlag(EInventoryFlag::OnlyX) && HasInventoryFlag(EInventoryFlag::LimitY))
		if (Position.Y + Size.Y > MaXCountRow)
		{
			Index = INDEX_NONE;
			return false;
		}

	for (int32 i = 0; i < Inventory.Items.Num(); i++)
	{
		if (Inventory.Items[i].ItemAsset) 
			if (Inventory.Items[i].IsPosition(Position, Size)) {
				Index = i;
				return false;
			}
	}

	return true;
}

void UInventoryComponent::SetRotateSlot(int32 Index, bool NewRotate)
{
	if (!Inventory.Items.IsValidIndex(Index) || Inventory.Items[Index].IsRotate == NewRotate || !GIsServer)
		return;

	auto CuToPosition = Inventory.Items[Index].PositionSlot;
	bool L_OldRotate = Inventory.Items[Index].IsRotate;
	int32 L_Index;

	Inventory.Items[Index].PositionSlot = Inventory.Items[Index].GetSize() * -2;
	Inventory.Items[Index].IsRotate = NewRotate;

	if (!IsPositionFree(CuToPosition, Inventory.Items[Index].GetSize(), L_Index)){
		Inventory.Items[Index].IsRotate = L_OldRotate;
		Inventory.Items[Index].PositionSlot = CuToPosition;
		return;
	}

	Inventory.Items[Index].PositionSlot = CuToPosition;

	ChangeSlot(Index, Inventory.Items[Index], ETypeSetItem::ChangeSlot);

}

bool UInventoryComponent::DropItem(int32 IndexItem = 0, int32 ToIndex = INDEX_NONE, int32 Count = 1, FIntPoint ToPosition = FIntPoint::ZeroValue, bool Change = false, bool FindPosition = false, bool Rotate = false)
{
	if (!Inventory.Items.IsValidIndex(IndexItem) && IndexItem == ToIndex) return false;

	if (ToIndex != INDEX_NONE) {

		if (Count <= 0 || Count > Inventory.Items[IndexItem].Count) return false;

		if (StackItem(IndexItem, ToIndex, Count)) {
			return true;
		}
		else if (Change) 
			return ChangeItem(IndexItem, ToIndex, FindPosition);
	}

	return SetPositionItem(IndexItem, Count, Rotate, ToPosition);
}

bool UInventoryComponent::ChangeItem(int32 IndexItem, int32 ToIndex, bool FindPosition)
{
	int32 Index;

	auto CuToPosition = Inventory.Items[ToIndex].PositionSlot;

	auto CuPosition = Inventory.Items[IndexItem].PositionSlot;

	Inventory.Items[ToIndex].PositionSlot = Inventory.Items[ToIndex].GetSize() * -2;
	Inventory.Items[IndexItem].PositionSlot = Inventory.Items[IndexItem].GetSize() * -2;

	if (IsPositionFree(CuToPosition, Inventory.Items[IndexItem].GetSize(), Index)) {

		Inventory.Items[IndexItem].PositionSlot = CuToPosition;

		if (IsPositionFree(CuPosition, Inventory.Items[ToIndex].GetSize(), Index)) {

			Inventory.Items[ToIndex].PositionSlot = CuPosition;

			ChangeSlot(ToIndex, Inventory.Items[ToIndex], ETypeSetItem::ChangeSlot);
			ChangeSlot(IndexItem, Inventory.Items[IndexItem], ETypeSetItem::ChangeSlot);
			return true;
		}
		else if (FindPosition) {

			FIntPoint NewPosition;

			if (FindFreeSlot(Inventory.Items[ToIndex].GetSize(), NewPosition))
			{
				Inventory.Items[ToIndex].PositionSlot = NewPosition;

				ChangeSlot(ToIndex, Inventory.Items[ToIndex], ETypeSetItem::ChangeSlot);
				ChangeSlot(IndexItem, Inventory.Items[IndexItem], ETypeSetItem::ChangeSlot);
				return true;
			}
		}
	}

	Inventory.Items[ToIndex].PositionSlot = CuToPosition;
	Inventory.Items[IndexItem].PositionSlot = CuPosition;

	return false;
}

bool UInventoryComponent::StackItem(int32 IndexItem, int32 ToIndex, int32 Count) {

	if (!Inventory.Items[IndexItem].GetData().StackItem)
		return false;

	if (Inventory.Items[IndexItem] == Inventory.Items[ToIndex] && Inventory.Items[ToIndex].Count + Count <= Inventory.Items[ToIndex].GetData().MaxStack)
	{
		Inventory.Items[IndexItem].Count -= Count;
		Inventory.Items[ToIndex].Count += Count;

		ChangeSlot(ToIndex, Inventory.Items[ToIndex], ETypeSetItem::ChangeSlot);

		bool IsCountRemove = Inventory.Items[IndexItem].Count == 0;

		if (IsCountRemove) {
			Inventory.Items.RemoveAt(IndexItem);
			ChangeSlot(IndexItem, FInventorySlot(), ETypeSetItem::Remove);
		}
		else
			ChangeSlot(IndexItem, Inventory.Items[IndexItem], ETypeSetItem::ChangeSlot);

		return true;
	}

	return false;
}

bool UInventoryComponent::SetPositionItem(int32 IndexItem,int32 Count, bool Rotate, FIntPoint NewPosition)
{
	if (NewPosition.X + Inventory.Items[IndexItem].GetSize(Rotate).X > GetCountRow(NewPosition.Y + Inventory.Items[IndexItem].GetSize(Rotate).Y == MaXCountRow)) return false;

	if (HasInventoryFlag(EInventoryFlag::LimitY))
		if (NewPosition.Y + Inventory.Items[IndexItem].GetSize(Rotate).Y > MaXCountRow) return false;

	for (int32 i = 0; i < Inventory.Items.Num(); i++)
	{
		if (i == IndexItem && Inventory.Items[i].Count == Count)
			continue;

		if (Inventory.Items[i].IsPosition(NewPosition, Inventory.Items[IndexItem].GetSize(Rotate))) {
			return false;
		}
	}

	if (Inventory.Items[IndexItem].GetData().StackItem) {
		if (Inventory.Items[IndexItem].Count != 1) {

			if (Inventory.Items[IndexItem].Count == Count) {

				Inventory.Items[IndexItem].PositionSlot = NewPosition;
				Inventory.Items[IndexItem].IsRotate = Rotate;
				ChangeSlot(IndexItem, Inventory.Items[IndexItem], ETypeSetItem::ChangeSlot);
				return true;
			}

			FInventorySlot Newslot = Inventory.Items[IndexItem];
			Newslot.Count = Count;
			Newslot.IsRotate = Rotate;
			Newslot.PositionSlot = NewPosition;

			Inventory.Items[IndexItem].Count -= Count;

			ChangeSlot(IndexItem, Inventory.Items[IndexItem], ETypeSetItem::ChangeSlot);

			int32 NewIndex = Inventory.Items.Add(Newslot);
			ChangeSlot(NewIndex, Inventory.Items[NewIndex], ETypeSetItem::Add);
			return true;
		}
	}

	Inventory.Items[IndexItem].PositionSlot = NewPosition;
	Inventory.Items[IndexItem].IsRotate = Rotate;
	ChangeSlot(IndexItem, Inventory.Items[IndexItem], ETypeSetItem::ChangeSlot);
	return true;
}

#if WITH_EDITOR
void UInventoryComponent::RecalculationMass() {
	
	if (!HasInventoryFlag(EInventoryFlag::Mass))
		return;

	Inventory.Massa = 0;

	for (auto& Element : Inventory.Items) {
		if(Element.ItemAsset)
			Inventory.Massa += Element.GetData().MassItem * Element.Count;
	}
}
#endif