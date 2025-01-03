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


FIntPoint FInventorySlot::GetSize()
{
	if (!HasInventoryFlag(EInventoryFlag::Size)) {
		return FIntPoint(1);
	}

	if (!IsValid(ItemAsset))
		return FIntPoint::ZeroValue;

	FIntPoint L_ReturnValue = ItemAsset->SlotItemData.SizeSlot;

	if (IsRotate)
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

	if (!NewValue.ItemAsset || !Inventory.Items.IsValidIndex(Index) || NewValue.Count <= 0 || NewValue.Count > NewValue.ItemAsset->SlotItemData.MaxStack)
		return false;

	if (Inventory.Items[Index].ItemAsset != NewValue.ItemAsset || Inventory.Items[Index].PositionSlot != NewValue.PositionSlot)
	{
		for (int32 i = 0; i < Inventory.Items.Num(); i++)
		{
			if (HasInventoryFlag(EInventoryFlag::Stack))
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
			Inventory.Massa -= Inventory.Items[Index].ItemAsset->SlotItemData.MassItem * Inventory.Items[Index].Count;
			Inventory.Massa += NewValue.ItemAsset->SlotItemData.MassItem * NewValue.Count;
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

	FIntPoint NewPosition;
	int32 IndexFull;

	if (!IsPositionFree(NewSlot.PositionSlot, NewSlot.GetSize(), IndexFull)) {

		if (!FindPositionSlot) {
			return false;
		}

		if (FindFreeSlot(NewSlot.GetSize(), NewPosition)) {
			NewSlot.PositionSlot = NewPosition;
		}
		else {
			return false;
		}
	}

	if (HasInventoryFlag(EInventoryFlag::Stack)) {

		if (!NewSlot.ItemAsset->SlotItemData.StackItem && NewSlot.Count > NewSlot.ItemAsset->SlotItemData.MaxStack) {
			return false;
		}
		else NewSlot.Count = 1;
	}
	else {
		NewSlot.Count = 1;
	}


	if (HasInventoryFlag(EInventoryFlag::Mass)) {
		Inventory.Massa += NewSlot.ItemAsset->SlotItemData.MassItem * NewSlot.Count;
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

	if (HasInventoryFlag(EInventoryFlag::Stack) && ItemAsset->SlotItemData.StackItem) {
		
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
	bool LimitY = !HasInventoryFlag(EInventoryFlag::OnlyX) ? HasInventoryFlag(EInventoryFlag::LimitY) ? Size.Y <= MaXCountRow : true : true;

	if (Size.X <= Inventory.CountRow  && LimitY) {
		if (Inventory.Items.Num() == 0) {
			ReturnPosition = FIntPoint::ZeroValue;
			return true;
		}
	}
	else {
		return false;
	}

	int32 X = 0, Y = 0;

	bool IsNoFree;

	do {
		if (HasInventoryFlag(EInventoryFlag::OnlyX)) {
			if (X + (Size.X - 1) > (GetCountRow(Size.Y + Y == MaXCountRow) - 1)) {
				return false;
			}
		}
		else {
			if (X + (Size.X - 1) > (GetCountRow(Size.Y + Y == MaXCountRow) - 1)) {
				X = 0;

				if (HasInventoryFlag(EInventoryFlag::LimitY))
					if (Y + (Size.Y - 1) > (MaXCountRow - 1))
						return false;
				Y++;
			}
		}

		int32 L_ItemIndex;

		IsNoFree = !IsPositionFree(FIntPoint(X, Y), Size, L_ItemIndex);

		if (IsNoFree) {

			if (HasInventoryFlag(EInventoryFlag::Size) && Inventory.Items.IsValidIndex(L_ItemIndex)) 
				X = Inventory.Items[L_ItemIndex].PositionSlot.X + Inventory.Items[L_ItemIndex].GetSize().X;	
			else 
				X++;
			
		}

	} while (IsNoFree);

	ReturnPosition = FIntPoint(X, Y);
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

	if (HasInventoryFlag(EInventoryFlag::Stack) && Inventory.Items[Index].ItemAsset->SlotItemData.StackItem) {

		if (Inventory.Items[Index].Count >= Count) {

			if (HasInventoryFlag(EInventoryFlag::Mass))
				Inventory.Massa -= Inventory.Items[Index].ItemAsset->SlotItemData.MassItem * Count;

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
		Inventory.Massa -= Inventory.Items[Index].ItemAsset->SlotItemData.MassItem;

	OnRemoveItem.Broadcast(Index);
	Inventory.Items.RemoveAt(Index);
	ChangeSlot(Index, FInventorySlot(), ETypeSetItem::Remove);

	return true;
}

bool UInventoryComponent::SendItem(int32 Index, UInventoryComponent* ToIntentory, int32 Count, bool FindSlot, FIntPoint Position)
{
	if(!ToIntentory|| !Inventory.Items.IsValidIndex(Index) || this == ToIntentory || Inventory.Items[Index].Count < Count) return false;

	FInventorySlot NewStot = Inventory.Items[Index];
	FIntPoint NewPosition = Position;
	int32 FindIndex;

	if (FindSlot) {
		if (!ToIntentory->FindFreeSlot(NewStot.GetSize(), NewPosition)) 
			return false;
	}
	else {
		if (!ToIntentory->IsPositionFree(NewPosition, NewStot.GetSize(), FindIndex)) 
			return false;
	}

	RemoveItem(Index, Count);

	NewStot.Count = Count;
	NewStot.PositionSlot = NewPosition;
	int32 AddIndex;
	ToIntentory->AddSlot(NewStot, false, AddIndex);
	return true;
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

bool UInventoryComponent::DropItem(int32 IndexItem = 0, int32 ToIndex = INDEX_NONE, int32 Count = 1, FIntPoint ToPosition = FIntPoint::ZeroValue, bool Change = false, bool FindPosition = false)
{
	if (!Inventory.Items.IsValidIndex(IndexItem)) return false;

	if (ToIndex != INDEX_NONE) {
		
		if (!HasInventoryFlag(EInventoryFlag::Stack))
		{
			if (Change) {
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
			}

			return false;
		}

		if (Count <= 0 || Count > Inventory.Items[IndexItem].Count) return false;

		if (IndexItem != ToIndex) {
			if (Inventory.Items[IndexItem].ItemAsset->SlotItemData.StackItem && Inventory.Items[IndexItem] == Inventory.Items[ToIndex] && Inventory.Items[ToIndex].Count + Count <= Inventory.Items[ToIndex].ItemAsset->SlotItemData.MaxStack)
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
			else if (Change) {

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
			}
		}
	}
	else {
		if (ToPosition.X + Inventory.Items[IndexItem].GetSize().X > GetCountRow(ToPosition.Y + Inventory.Items[IndexItem].GetSize().Y == MaXCountRow)) return false;

		if (HasInventoryFlag(EInventoryFlag::LimitY))
			if (ToPosition.Y + Inventory.Items[IndexItem].GetSize().Y > MaXCountRow) return false;

		for (int32 i = 0; i < Inventory.Items.Num(); i++)
		{
			if (i == IndexItem && Inventory.Items[i].Count == Count)
				continue;

			if (Inventory.Items[i].IsPosition(ToPosition, Inventory.Items[IndexItem].GetSize())) {
				return false;
			}
		}

		if (HasInventoryFlag(EInventoryFlag::Stack)) {
			if (Inventory.Items[IndexItem].Count != 1) {

				FInventorySlot Newslot = Inventory.Items[IndexItem];
				Newslot.Count = Count;
				Newslot.PositionSlot = ToPosition;

				Inventory.Items[IndexItem].Count -= Count;

				if (Inventory.Items[IndexItem].Count == 0) {
					Inventory.Items.RemoveAt(IndexItem);
					ChangeSlot(IndexItem, FInventorySlot(), ETypeSetItem::Remove);
				}
				else {
					ChangeSlot(IndexItem, Inventory.Items[IndexItem], ETypeSetItem::ChangeSlot);
				}

				int32 NewIndex = Inventory.Items.Add(Newslot);
				ChangeSlot(NewIndex, Inventory.Items[NewIndex], ETypeSetItem::Add);
				return true;
			}
		}

		Inventory.Items[IndexItem].PositionSlot = ToPosition;
		ChangeSlot(IndexItem, Inventory.Items[IndexItem], ETypeSetItem::SetPosition);
		return true;
	}

	return false;
}

#if WITH_EDITOR
void UInventoryComponent::RecalculationMass() {
	
	if (!HasInventoryFlag(EInventoryFlag::Mass))
		return;

	Inventory.Massa = 0;

	for (auto& Element : Inventory.Items) {
		if(Element.ItemAsset)
			Inventory.Massa += Element.ItemAsset->SlotItemData.MassItem * Element.Count;
	}
}
#endif