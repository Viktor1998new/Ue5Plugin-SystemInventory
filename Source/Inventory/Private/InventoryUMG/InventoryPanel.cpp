//Copyright(c) 2022 - 2025, Viktor.F.P

#include "InventoryUMG/InventoryPanel.h"
#include "InventoryUMG/InventoryPanelSlot.h"

#define LOCTEXT_NAMESPACE "UMG"

UInventoryPanel::UInventoryPanel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UInventoryPanel::RemoveFromParent() {

	if (Inventory)
		Inventory->NewDataSlot.RemoveDynamic(this, &UInventoryPanel::OnChangeSlot);

	Super::RemoveFromParent();
}

void UInventoryPanel::SetInventory(UInventoryComponent * NewInventory)
{
	if (Inventory) {
		Inventory->NewDataSlot.RemoveDynamic(this, &UInventoryPanel::OnChangeSlot);
		ClearChildren();
	}

	if (!NewInventory) return;

	Inventory = NewInventory;

	Inventory->NewDataSlot.AddDynamic(this, &UInventoryPanel::OnChangeSlot);
}

UInventoryPanelSlot* UInventoryPanel::SlotAsInventorySlot(UWidget* Widget)
{
	if (Widget)	return Cast<UInventoryPanelSlot>(Widget->Slot);

	return nullptr;
}

UClass* UInventoryPanel::GetSlotClass() const
{
	return UInventoryPanelSlot::StaticClass();
}

void UInventoryPanel::OnSlotAdded(UPanelSlot* inSlot)
{
	if (!MyPanel.IsValid()) return;

	CastChecked<UInventoryPanelSlot>(inSlot)->Parent = this;
	CastChecked<UInventoryPanelSlot>(inSlot)->BuildSlot(MyPanel.ToSharedRef());
}

void UInventoryPanel::OnSlotRemoved(UPanelSlot* inSlot)
{
	if (!MyPanel.IsValid()) return;

	TSharedPtr<SWidget> Widget = inSlot->Content->GetCachedWidget();

	if (Widget.IsValid()) ItemSlots.Remove(Cast<UInventoryPanelSlot>(inSlot));

	MyPanel->RemoveSlot(Widget.ToSharedRef());
}

UInventoryPanelSlot* UInventoryPanel::AddChildToInventoryPanelItem(UWidget* Content, int32 Index)
{
	UInventoryPanelSlot* PanelSlot = Cast<UInventoryPanelSlot>(Super::AddChild(Content));

	if (PanelSlot != nullptr)
	{
		PanelSlot->SetIndexItem(Index);
		ItemSlots.Add(PanelSlot);
	}

	return PanelSlot;

}

UInventoryPanelSlot* UInventoryPanel::AddChildToInventoryPanel(UWidget* Content)
{
	
	return Cast<UInventoryPanelSlot>(Super::AddChild(Content));

}

void UInventoryPanel::ReleaseSlateResources(bool bReleaseChildren)
{
	if (Inventory)
		Inventory->NewDataSlot.RemoveDynamic(this, &UInventoryPanel::OnChangeSlot);

	Super::ReleaseSlateResources(bReleaseChildren);

	MyPanel.Reset();
}

TSharedRef<SWidget> UInventoryPanel::RebuildWidget()
{
	MyPanel = SNew(SConstraintCanvas);

	if (Slots.Num() != 0 && Inventory)
		for (UPanelSlot* PanelSlot : Slots)
		{
			if (UInventoryPanelSlot* TypedSlot = Cast<UInventoryPanelSlot>(PanelSlot))
			{
				TypedSlot->Parent = this;
				TypedSlot->BuildSlot(MyPanel.ToSharedRef());
			}
		}

	return MyPanel.ToSharedRef();
}



#if WITH_EDITOR
const FText UInventoryPanel::GetPaletteCategory()
{
	return LOCTEXT("InventoryPanel", "InventoryPanel");
}

#endif

#undef LOCTEXT_NAMESPACE
