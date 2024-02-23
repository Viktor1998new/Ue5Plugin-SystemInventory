//Copyright(c) 2022, Viktor.F.P

#include "InventoryUMG/InventoryGrid.h"
#include "InventoryUMG/InventoryPanelSlot.h"
#include "InventorySettings.h"
#include "GameFramework/PlayerState.h"
#include "TimerManager.h"
#include "Inventory.h"

#define LOCTEXT_NAMESPACE "UMG"

UInventoryGrid::UInventoryGrid(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UInventoryGrid::SetInventory(UInventoryComponent* NewInventory)
{
	Super::SetInventory(NewInventory);

	if (!IsValid(NewInventory))
		return;

	if (NoneSlot && Inventory->Inventory.MaxSlot != 0) {
		if (EnumHasAnyFlags((EInventoryFlag)UInventorySettings::Get()->InventoryFlags, EInventoryFlag::OnlyX)) {
			for (int32 X = 0; X < Inventory->Inventory.MaxSlot; X++)
				AddNoneSlot(FIntPoint(X, 0));
		}
		else {
			for (int32 Y = 0; Y < Inventory->Inventory.MaxSlot; Y++)
					AddNoneSlot(FIntPoint(Y % Inventory->Inventory.CountRow, Y / Inventory->Inventory.CountRow));
		}
	}

	if (Inventory->CountItems() != 0) {

		for (int32 i = 0; i < Inventory->CountItems(); i++)
			if (Inventory->GetItem(i).ItemAsset)
				AddSlot(i);
	}
}

void UInventoryGrid::AddNoneSlot(FIntPoint Position)
{
	UUserWidget* WidgetSlot = CreateWidget<UUserWidget>(this, NoneSlot);
	UInventoryPanelSlot* NewSlot = Cast<UInventoryPanelSlot>(Super::AddChild(WidgetSlot));
	SetSlotTranstrorm(NewSlot, Position, E_Position);
	SetSlotTranstrorm(NewSlot, FIntPoint(1) ,E_Size);
}

void UInventoryGrid::AddSlot(int32 IndexItem)
{
	UUserWidget* WidgetSlot = CreateWidget<UUserWidget>(this,ItemSlot);
	UInventoryPanelSlot* NewSlot = Cast<UInventoryPanelSlot>(Super::AddChild(WidgetSlot));
	FInventorySlot L_ItemSlot = Inventory->GetItem(IndexItem);
	SetSlotTranstrorm(NewSlot, L_ItemSlot.PositionSlot, E_Position);
	SetSlotTranstrorm(NewSlot, L_ItemSlot.GetSize(), E_Size);
	NewSlot->SetIndexItem(IndexItem);
	NewSlot->SetZOrder(1);
	ItemSlots.Add(NewSlot);
}

void UInventoryGrid::SetSlotTranstrorm(UInventoryPanelSlot* ChangeSlot ,FIntPoint Value, TypeChangeTranstrorm Change) {

	if (!ChangeSlot) return;

	FMargin Offsets = ChangeSlot->Transform.Offsets;

	switch (Change) {

	case E_Position:

		Offsets.Left = Value.X * SizeSlot;
		Offsets.Top = Value.Y * SizeSlot;
		break;


	case E_Size:

		Offsets.Right = Value.X * SizeSlot;
		Offsets.Bottom = Value.Y * SizeSlot;
		break;
	}


	ChangeSlot->SetMargin(Offsets);
}

void UInventoryGrid::OnChangeSlot(int32 Index, FInventorySlot NewData, ETypeSetItem Type)
{	
	if(GetOwningPlayer()){
		if (Inventory->GetIsReplicated()) {
			float TimePing = GetOwningPlayer()->PlayerState->GetPing() * GetWorld()->GetDeltaSeconds();

			if (TimePing == 0.0f) {
				ChangeSlots(Index, NewData, Type);
				return;
			}

			FTimerHandle TimerPing;
			FTimerDelegate Del_ChangeSlots = FTimerDelegate::CreateUObject(this, &UInventoryGrid::ChangeSlots, Index,NewData,Type);
			GetWorld()->GetTimerManager().SetTimer(TimerPing, Del_ChangeSlots, TimePing, false);
			return;
		}
	}

	ChangeSlots(Index, NewData, Type);
}

void UInventoryGrid::ChangeSlots(int32 Index, FInventorySlot NewData, ETypeSetItem Type) {

	switch (Type)
	{
	case ETypeSetItem::Add:
		AddSlot(Index);
		break;

	case ETypeSetItem::Remove:

		RemoveChild(ItemSlots.Last()->Content);

		if (Inventory->CountItems() == 0)
			return;

		for (int32 i = Index; i < ItemSlots.Num(); i++) {
			FInventorySlot L_Slot = Inventory->GetItem(i);
			ItemSlots[i]->ChangeSlot(L_Slot);
			SetSlotTranstrorm(ItemSlots[i], L_Slot.PositionSlot, E_Position);
			SetSlotTranstrorm(ItemSlots[i], L_Slot.GetSize(), E_Size);
		}

		break;

	case ETypeSetItem::ChangeSlot:
		ItemSlots[Index]->ChangeSlot(NewData);
		SetSlotTranstrorm(ItemSlots[Index], NewData.PositionSlot, E_Position);
		SetSlotTranstrorm(ItemSlots[Index], NewData.GetSize(), E_Size);
		break;

	case ETypeSetItem::SetPosition:
		SetSlotTranstrorm(ItemSlots[Index], NewData.PositionSlot, E_Position);
		break;
	}
}


#undef LOCTEXT_NAMESPACE