//Copyright(c) 2022, Viktor.F.P
#include "SlotsWidget.h"
#include "Inventory.h"
#include "InventorySettings.h"
#include "Widgets/Layout/SBorder.h"
#include "InventoryUMG/InventoryPanel.h"
#include "InventoryUMG/InventoryPanelSlot.h"
#include "Brushes/SlateColorBrush.h"
#include "Widgets/Input/SMenuAnchor.h"
#include "Widgets/Images/SImage.h"
#include "PropertyEditorModule.h"
#include "InventoryLibrary.h"
#include "Components/MenuAnchor.h"
#include "Widgets/Layout/SScaleBox.h"

FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

float SizeSlot = 32.f;

UInventoryComponent* USlotWidget::GetInventory()
{
	return Cast<UInventoryPanel>(GetParent())->Inventory;
}

bool USlotNoneWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UInventoryPanelSlot* L_Slot = Cast<UInventoryPanelSlot>(Slot);

	if (!IsValid(L_Slot))
		return false;

	FIntPoint L_CurrentPositin = FIntPoint(L_Slot->Transform.Offsets.Left / SizeSlot, L_Slot->Transform.Offsets.Top / SizeSlot);
	UInventoryComponent* L_Inventory = GetInventory();

	if (auto Operation = Cast<UEditor_Drag>(InOperation)) {

		if(Operation->NewItem){
			FInventorySlot LNewSlot = Operation->Slot;
			LNewSlot.PositionSlot = L_CurrentPositin;
			int32 L_Index;
			L_Inventory->AddSlot(LNewSlot, false, L_Index);
		}
		else {
			if (Operation->Inventory == L_Inventory) {
				bool OldRotation = Operation->Inventory->GetItem(Operation->Index).IsRotate;
				auto* L_SlotItem = &Operation->Inventory->GetItem(Operation->Index);
				L_SlotItem->IsRotate = Operation->IsRotate;
				if (Operation->Inventory->DropItem(Operation->Index, -1, 1, L_CurrentPositin, true, true)) {
					if (L_SlotItem->Count != 1) {
						L_SlotItem->IsRotate = OldRotation;
					}
					Operation->Inventory->NewDataSlot.Broadcast(Operation->Index, Operation->GetSlot(), ETypeSetItem::ChangeSlot);
				}
				else {
					L_SlotItem->IsRotate = OldRotation;
				}
				Cast<USlotItemWidget>(Operation->Payload)->SetVisible();
				return true;
			}

			Operation->Inventory->GetItem(Operation->Index).IsRotate = Operation->IsRotate;
			Operation->Inventory->SendItem(Operation->Index, L_Inventory, 1, false, L_CurrentPositin);
			Cast<USlotItemWidget>(Operation->Payload)->SetVisible();
			return true;
		}
	}

	return true;
}

TSharedRef<SWidget> USlotNoneWidget::RebuildWidget()
{
	FSlateColorBrush* BrushButtons = new  FSlateColorBrush(FLinearColor(0.0f, 0.0f, 0.0f));

	MyPanel = SNew(SConstraintCanvas)
		+ SConstraintCanvas::Slot()
		.Anchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f))
		.Offset(FMargin(1.5f, 1.5f, 1.5f, 1.5f))
		[
			SNew(SBorder)
				.BorderImage(BrushButtons)
		];

	return  MyPanel.ToSharedRef();
}

FMargin USlotItemWidget::GetOffsetMouse() const
{
	return FMargin(MousePosition.X, MousePosition.Y);
}

void USlotItemWidget::SetVisible()
{
	SetVisibility(ESlateVisibility::Visible);
	ImageItem->SetVisibility(TAttribute<EVisibility>(EVisibility::Visible));
}

void USlotItemWidget::NativePreConstruct()
{
	UInventoryPanelSlot* L_Slot = Cast<UInventoryPanelSlot>(Slot);
	L_Slot->OnChangedSlot.AddDynamic(this, &USlotItemWidget::OnChangedSlot);
}

FReply USlotItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton)) {
		
		SetKeyboardFocus();

		TSharedPtr<SWidget> SlateWidgetDetectingDrag = GetCachedWidget();

		if (SlateWidgetDetectingDrag.IsValid())
		{
			return FReply::Handled().DetectDrag(SlateWidgetDetectingDrag.ToSharedRef(), EKeys::LeftMouseButton);
		}
	}
	
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton)) {
		MousePosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		MenuAnchor->SetIsOpen(true, true);
		ContextMenu->SetItem(MenuAnchor, Cast<UInventoryPanel>(GetParent())->Inventory, Item_Index);

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void USlotItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	operation = NewObject<UEditor_Drag>();

	operation->Pivot = EDragPivot::TopLeft;

	auto DragVisual = CreateWidget<UVisualDragWidget>(this);
	
	operation->Payload = this;

	UItemAsset* L_Asset = Item_Slot.ItemAsset;
	DragVisual->SetItem(L_Asset->SlotItemData.ImageItem, Item_Slot.GetSize() * SizeSlot);

	operation->DefaultDragVisual = DragVisual;

	operation->Index = Item_Index;
	operation->IsRotate = Item_Slot.IsRotate;

	operation->Inventory = Cast<UInventoryPanel>(GetParent())->Inventory;

	if (Item_Slot.Count - 1 <= 0)
	{
		SetVisibility(ESlateVisibility::HitTestInvisible);
		ImageItem->SetVisibility(TAttribute<EVisibility>(EVisibility::Hidden));
	}

	OutOperation = operation;
}

void USlotItemWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	SetVisible();
}

FReply USlotItemWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftShift) {
		if (IsValid(operation)) {
			operation->RotateItem();
		}
	}
	return FReply::Handled();
}

void USlotItemWidget::OnChangedSlot(int32 NewIndex, FInventorySlot NewSlot)
{
	Item_Index = NewIndex;
	Item_Slot = NewSlot;

	FSlateBrush* BrushButtons = new FSlateBrush();
	BrushButtons->TintColor = FLinearColor(1.0f, 1.0f, 1.0f);
	BrushButtons->SetResourceObject(NewSlot.ItemAsset->SlotItemData.ImageItem);
	ImageItem->SetBorderImage(BrushButtons);

	if (NewSlot.ItemAsset->SlotItemData.StackItem)
		NumberText->SetText(FText::AsNumber(NewSlot.Count));
	else
		NumberText->SetText(FText::FromString(""));

	SetVisible();
}

bool USlotItemWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{

	bool IsSussen = false;
	if (UInventoryPanelSlot* L_PanelSlot = Cast<UInventoryPanelSlot>(Slot)) {

		if (UEditor_Drag* Drag = Cast<UEditor_Drag>(InOperation)) {

			if (Drag->NewItem)
				return false;

			if (auto L_Inventory = GetInventory()) {
				if (L_Inventory == Drag->Inventory) {
					Drag->Inventory->DropItem(Drag->Index, Item_Index, 1, FIntPoint(), true, true);

					IsSussen = true;
				}
				else {
					FInventorySlot Drop_Slot = Drag->GetSlot();

					bool IsQAsset = Drop_Slot.ItemAsset == Item_Slot.ItemAsset;
					bool IsQData = Drop_Slot.ItemData == Item_Slot.ItemData;

					if (IsQAsset && IsQData && Drop_Slot.ItemAsset->SlotItemData.StackItem) {
						FInventorySlot New_Slot = Item_Slot;
						New_Slot.IsRotate = Drag->IsRotate;
						New_Slot.Count += 1;
						bool IsSet = L_Inventory->SetSlot(Item_Index, New_Slot);

						if (IsSet)
							IsSussen = Drag->Inventory->RemoveItem(Drag->Index, 1);
					}
				}
			}

			if (IsValid(Drag->Payload)) {
				Cast<USlotItemWidget>(Drag->Payload)->SetVisible();
			}
		}
	}
	return IsSussen;
}

TSharedRef<SWidget> USlotItemWidget::HandleGetMenuContent()
{
	UInventoryPanel* Panel = Cast<UInventoryPanel>(GetParent());

	ContextMenu = CreateWidget<UMenuContextItemWidget>(this);

	return ContextMenu->TakeWidget();
}

TSharedRef<SWidget> USlotItemWidget::RebuildWidget()
{
	FSlateFontInfo NumderFont = FCoreStyle::GetDefaultFontStyle("Roboto", 10);
	bIsFocusable = true;
	MyPanel = SNew(SConstraintCanvas).Visibility(EVisibility::Visible)
		+ SConstraintCanvas::Slot()
		.Anchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f))
		.Offset(FMargin(0.0f, 0.0f, 0.0f, 0.0f))
		[
			SAssignNew(ImageItem, SBorder)
				.Content()[
					SAssignNew(NumberText, STextBlock)
						.Font(NumderFont)
						.Text(FText::FromString(""))
				]
		];

	A_Offset.BindUObject(this, &USlotItemWidget::GetOffsetMouse);

	MyPanel->AddSlot()
		.Offset(A_Offset)
		.AutoSize(true)[
			SAssignNew(MenuAnchor, SMenuAnchor)
				.Placement(EMenuPlacement::MenuPlacement_ComboBox)
				.OnGetMenuContent(FOnGetContent::CreateUObject(this, &USlotItemWidget::HandleGetMenuContent))];

	
	return SNew(SBox).WidthOverride(32.f).HeightOverride(32.f) [ MyPanel.ToSharedRef()];
}

void UVisualDragWidget::SetItem(UTexture2D* Texture, FVector2D Size)
{

	if (SizeBox.IsValid()) {
		SizeBox.ToSharedRef()->SetWidthOverride(Size.X);
		SizeBox.ToSharedRef()->SetHeightOverride(Size.Y);
		return;
	}

	FSlateColorBrush* Brush = new  FSlateColorBrush(FLinearColor(0.0f, 0.0f, 0.0f, 0.6f));

	FSlateBrush* BrushTexure = new FSlateBrush();

	if (BrushTexure->GetResourceObject() != Texture)
	{
		BrushTexure->SetResourceObject(Texture);
		BrushTexure->DrawAs = ESlateBrushDrawType::Image;
		BrushTexure->ImageSize = Size;

		if (Texture)
		{
			Texture->bForceMiplevelsToBeResident = true;
			Texture->bIgnoreStreamingMipBias = true;
		}
	}

	SizeBox = SNew(SBox)
		.WidthOverride(Size.X)
		.HeightOverride(Size.Y)[
			SNew(SBorder)
				.BorderImage(Brush)[
					SNew(SImage)
						.Image(BrushTexure)
				]
		];
}

TSharedRef<SWidget> UVisualDragWidget::RebuildWidget()
{
	return SizeBox.ToSharedRef();
}

void USlotAssetWidget::SetAssetItem(UItemAsset* NewAsset)
{
	if (!NewAsset)
		return;

	Asset = NewAsset;
	UTexture2D* Texture = Asset->SlotItemData.ImageItem;

	FSlateBrush* Brush = new FSlateBrush();

	if (Brush->GetResourceObject() != Texture)
	{
		Brush->SetResourceObject(Texture);

		if (Texture)
		{
			Texture->bForceMiplevelsToBeResident = true;
			Texture->bIgnoreStreamingMipBias = true;
			Brush->ImageSize = FVector2D(64.f, 64.f);
		}

		ItemIcon.ToSharedRef()->SetImage(Brush);
	}

	NameAsset.ToSharedRef()->SetText(FText::FromString(Asset->GetName()));
}

FReply USlotAssetWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton)) {

		SetKeyboardFocus();

		TSharedPtr<SWidget> SlateWidgetDetectingDrag = GetCachedWidget();

		if (SlateWidgetDetectingDrag.IsValid())
		{
			return FReply::Handled().DetectDrag(SlateWidgetDetectingDrag.ToSharedRef(), EKeys::LeftMouseButton);
		}
	}
	return FReply::Handled();
}

void USlotAssetWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	operation = NewObject<UEditor_Drag>();

	operation->NewItem = true;
	operation->Pivot = EDragPivot::TopLeft;

	auto DragVisual = CreateWidget<UVisualDragWidget>(this);

	FIntPoint Size = Asset->SlotItemData.SizeSlot;

	if (!UInventorySettings::Get()->HasInventoryFlag(EInventoryFlag::Size))
		Size = FIntPoint(1);

	DragVisual->SetItem(Asset->SlotItemData.ImageItem, Size * SizeSlot);

	operation->DefaultDragVisual = DragVisual;

	FInventorySlot NewSlot;
	NewSlot.ItemAsset = Asset;
	NewSlot.Count = 1;

	operation->Slot = NewSlot;

	OutOperation = operation;
}

FReply USlotAssetWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftShift) {
		if (IsValid(operation)) {
			operation->RotateItem();
		}
	}
	return FReply::Handled();
}

TSharedRef<SWidget> USlotAssetWidget::RebuildWidget()
{
	bIsFocusable = true;

	FSlateColorBrush* Brush = new  FSlateColorBrush(FLinearColor(0.0f, 0.0f, 0.0f, 0.6f));
	FSlateFontInfo NumderFont = FCoreStyle::GetDefaultFontStyle("Roboto", 9);

	return SNew(SBox)
		.WidthOverride(128.0f)
		.HeightOverride(128.0f)
		.MaxDesiredHeight(128.0f)
		.MaxDesiredWidth(128.0f)
		[
			SNew(SBorder)
				.BorderImage(Brush)[
					SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.Padding(FMargin(10.f))[
							SAssignNew(ItemIcon, SImage)
						]

						+ SVerticalBox::Slot()
						.AutoHeight()[
							SNew(SSpacer)
								.Size(FVector2D(1.f, 5.f))
						]

						+ SVerticalBox::Slot()
						.FillHeight(1.f)[
							SAssignNew(NameAsset, STextBlock)
								.Font(NumderFont)
								.Text(FText::FromString(""))
								.Justification(ETextJustify::Center)
								.AutoWrapText(true)
								.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
								.Clipping(EWidgetClipping::ClipToBounds)

						]
				]
		];
}

void UMenuContextItemWidget::SetItem(TSharedPtr<class SMenuAnchor> MewMenu, UInventoryComponent* NewInventory, int32 NewIndex)
{
	FInventorySlot L_SlotItem = NewInventory->GetItem(NewIndex);

	ItemSettings->Asset = L_SlotItem.ItemAsset;
	ItemSettings->Data = UInventoryLibrary::DataItemToMap(L_SlotItem.ItemData);
	ItemSettings->Count = L_SlotItem.Count;

	Inventory = NewInventory;
	Index = NewIndex;
	Menu = MewMenu;
}

FReply UMenuContextItemWidget::OnClickedRemoveItem()
{
	Inventory->RemoveItem(Index, Inventory->GetItem(Index).Count);
	Menu->SetIsOpen(false, false);
	return FReply::Handled();
}

TSharedRef<SWidget> UMenuContextItemWidget::RebuildWidget()
{
	FSlateFontInfo NumderFont = FCoreStyle::GetDefaultFontStyle("Roboto", 9);

	ItemSettings = NewObject<UItemSettings>();
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bUpdatesFromSelection = false;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;


	TSharedPtr<IDetailsView> MyDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	MyDetailsView->SetObject(ItemSettings);
	MyDetailsView->OnFinishedChangingProperties().AddLambda([this](const FPropertyChangedEvent& Property) {

		if (!Inventory)
			return;

		if (!ItemSettings->Asset) {
			ItemSettings->Asset = Inventory->GetItem(Index).ItemAsset;
			return;
		}

		FInventorySlot L_SlotItem = Inventory->GetItem(Index);
		FInventorySlot L_NewSlotItem;
		L_NewSlotItem.ItemAsset = ItemSettings->Asset;
		L_NewSlotItem.ItemData = UInventoryLibrary::DataItem(ItemSettings->Data);
		L_NewSlotItem.PositionSlot = L_SlotItem.PositionSlot;
		L_NewSlotItem.Count = L_NewSlotItem.ItemAsset->SlotItemData.StackItem ? ItemSettings->Count : 1;
		L_NewSlotItem.IsRotate = L_SlotItem.IsRotate;

		if (!Inventory->SetSlot(Index, L_NewSlotItem)) {

			ItemSettings->Asset = L_SlotItem.ItemAsset;
			ItemSettings->Count = L_SlotItem.Count;
			ItemSettings->Data = UInventoryLibrary::DataItemToMap(L_SlotItem.ItemData);
			return;
		}
	});


	return SNew(SBox)[
		SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)[
				MyDetailsView.ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()[
				SNew(SButton)
					.OnClicked(FOnClicked::CreateUObject(this, &UMenuContextItemWidget::OnClickedRemoveItem))
					.Content()
					[
						SNew(STextBlock)
							.Font(NumderFont)
							.Text(FText::FromString("Remove Item"))
					].HAlign(HAlign_Center).VAlign(VAlign_Center)
			]
	];
}

FMargin USlotItemListWidget::GetOffsetMouse() const
{
	return FMargin(MousePosition.X, MousePosition.Y);
}

void USlotItemListWidget::NativePreConstruct()
{
	UInventoryPanelSlot* L_Slot = Cast<UInventoryPanelSlot>(Slot);
	L_Slot->OnChangedSlot.AddDynamic(this, &USlotItemListWidget::OnChangedSlot);
}

FReply USlotItemListWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{

	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton)) {

		MousePosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		MenuAnchor->SetIsOpen(true, true);
		ContextMenu->SetItem(MenuAnchor, Cast<UInventoryPanel>(GetParent())->Inventory, Item_Index);
	}

	return FReply::Handled();
}

void USlotItemListWidget::OnChangedSlot(int32 NewIndex, FInventorySlot NewSlot)
{
	Item_Index = NewIndex;

	if (!NewSlot.ItemAsset)
		return;

	UItemAsset* Asset = NewSlot.ItemAsset;
	UTexture2D* Texture = Asset->SlotItemData.ImageItem;

	FSlateBrush* Brush = new FSlateBrush();

	if (Brush->GetResourceObject() != Texture)
	{
		Brush->SetResourceObject(Texture);

		if (Texture)
		{
			Texture->bForceMiplevelsToBeResident = true;
			Texture->bIgnoreStreamingMipBias = true;
			Brush->ImageSize = FVector2D(32.f, 32.f);
		}
		if (Asset->SlotItemData.NameItem.IsEmpty())
			Text_Name.ToSharedRef()->SetText(FText::FromString("No Name"));
		else
			Text_Name.ToSharedRef()->SetText(Asset->SlotItemData.NameItem);

		auto L_Data = UInventoryLibrary::DataItemToMap(NewSlot.ItemData);

		FString StringResult;
		TArray<FString> Keys;
		L_Data.GetKeys(Keys);

		if (Keys.IsValidIndex(0)) {
			for (int32 i = 0; i < L_Data.Num(); i++) {

				FString AddValue = FString("Name: ") + Keys[i] + " " + FString("Value: ") + L_Data[Keys[i]];

				if (L_Data.Num() == (i + 1))
					StringResult += AddValue;
				else
					StringResult += AddValue + ",";
			}
			Text_Data.ToSharedRef()->SetText(FText::FromString(StringResult));
		}
		else
			Text_Data.ToSharedRef()->SetText(FText::FromString("No Data"));

		Image.ToSharedRef()->SetImage(Brush);
	}

}

TSharedRef<SWidget> USlotItemListWidget::HandleGetMenuContent()
{
	UInventoryPanel* Panel = Cast<UInventoryPanel>(GetParent());

	ContextMenu = CreateWidget<UMenuContextItemWidget>(this);

	return ContextMenu->TakeWidget();
}

TSharedRef<SWidget> USlotItemListWidget::RebuildWidget()
{
	FSlateColorBrush* BrushSlot = new  FSlateColorBrush(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f));
	FSlateFontInfo FontName = FCoreStyle::GetDefaultFontStyle("Roboto", 12);
	FSlateFontInfo FontData = FCoreStyle::GetDefaultFontStyle("Roboto", 10);

	TSharedPtr<SHorizontalBox> Horizontal = SNew(SHorizontalBox);

	Horizontal->AddSlot()
		.AutoWidth()
		.Padding(FMargin(10.0f, 0.0f, 0.0f, 0.0f))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)[
			SAssignNew(Image, SImage)
		];

	Horizontal->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Center)[
			SNew(SSpacer)
				.Size(FVector2D(20.0f, 0.0f))
		];

	Horizontal->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)[

			SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 5.0f, 0.0f, 0.0f))
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Fill)[
					SAssignNew(Text_Name, STextBlock)
						.Font(FontName)
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 10.0f, 0.0f, 0.0f))
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Fill)[
					SAssignNew(Text_Data, STextBlock)
						.Font(FontData)
				]
		];

	TSharedPtr<SConstraintCanvas> Panel = SNew(SConstraintCanvas)
		+ SConstraintCanvas::Slot()
		.AutoSize(true)
		.Anchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f))
		.Offset(FMargin(0.0f, 0.0f, 0.0f, 0.0f))[
			SNew(SBorder)
				.BorderImage(BrushSlot)[
					Horizontal.ToSharedRef()
				]
		];

	A_Offset.BindUObject(this, &USlotItemListWidget::GetOffsetMouse);

	Panel.ToSharedRef()->AddSlot()
		.Offset(A_Offset)
		.AutoSize(true)[
			SAssignNew(MenuAnchor, SMenuAnchor)
				.Placement(EMenuPlacement::MenuPlacement_ComboBox)
				.OnGetMenuContent(FOnGetContent::CreateUObject(this, &USlotItemListWidget::HandleGetMenuContent))];

	return Panel.ToSharedRef();
}

void UEditor_Drag::RotateItem() {
	IsRotate = !IsRotate;

	if (NewItem) {
		Slot.IsRotate = IsRotate;
		Cast<class UVisualDragWidget>(DefaultDragVisual)->SetItem(Slot.ItemAsset->SlotItemData.ImageItem, Slot.GetSize() * SizeSlot);
	}
	else {
		auto Item_Slot = GetSlot();
		Item_Slot.IsRotate = IsRotate;
		Cast<class UVisualDragWidget>(DefaultDragVisual)->SetItem(Item_Slot.ItemAsset->SlotItemData.ImageItem, Item_Slot.GetSize() * SizeSlot);
	}
}