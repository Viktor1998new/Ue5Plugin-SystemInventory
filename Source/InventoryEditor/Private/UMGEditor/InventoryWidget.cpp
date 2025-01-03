//Copyright(c) 2022 - 2025, Viktor.F.P

#include "UMGEditor/InventoryWidget.h"

#include "SlotsWidget.h"
#include "InventorySettings.h"
#include "Inventory/InventoryComponent.h"

#include "InventoryUMG/InventoryGrid.h"
#include "InventoryUMG/InventoryList.h"

#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Components/ScaleBox.h"
#include "Components/ScaleBoxSlot.h"
#include "Brushes/SlateColorBrush.h"

#define HasInventoryFlag(Flag) UInventorySettings::Get()->HasInventoryFlag(Flag)

FReply UInventoryWidget::RecalculationMass()
{
    if (HasInventoryFlag(EInventoryFlag::Mass))
        MassText->SetText(FText::Format(FText::FromString("Mass: {0}"), FText::AsNumber(Inventory->Inventory.Massa)));

    return FReply::Handled();
}

void UInventoryWidget::SwitchPanel(uint8 NewPanel)
{
    Panel = NewPanel;

    Switcher->SetActiveWidgetIndex(Panel);

    if (!IsValid(Inventory))
        return;

    switch (Panel)
    {
    case 0:

        InventoryList->SetInventory(nullptr);
        InventoryGrid->SetInventory(Inventory);
        break;

    case 1:

        InventoryList->SetInventory(Inventory);
        InventoryGrid->SetInventory(nullptr);
        break;
    }
}

bool UInventoryWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    if (Panel != 1)
        return false;

    if (auto Operation = Cast<UEditor_Drag>(InOperation)) {

        if (Operation->NewItem) {
            int32 L_Index;
            return Inventory->AddSlot(Operation->Slot, true, L_Index);
        }
    }

    return false;
}

void UInventoryWidget::SetInventory(UInventoryComponent* NewInventory, uint8 NewPanel)
{
    Inventory = NewInventory;

    SwitchPanel(NewPanel);

    TextNameActor->SetText(FText::Format(FText::FromString("Actor: {0}"), FText::FromString(Inventory->GetOwner()->GetName())));

    if (HasInventoryFlag(EInventoryFlag::Mass)) {
        Recalculation->SetVisibility(EVisibility::Visible);
        MassText->SetText(FText::Format(FText::FromString("Mass: {0}"), FText::AsNumber(Inventory->Inventory.Massa)));
    }
    else {
        Recalculation->SetVisibility(EVisibility::Hidden);
    }
}

TSharedRef<SWidget> UInventoryWidget::RebuildWidget()
{
    FSlateColorBrush* BrushButtons = new FSlateColorBrush(FLinearColor(0.0f, 0.0f, 0.0f));
    FSlateColorBrush* BrushInventory = new FSlateColorBrush(FLinearColor(0.01f, 0.01f, 0.01f));

    FSlateFontInfo TextFont = FCoreStyle::GetDefaultFontStyle("Roboto", 12);
    FSlateFontInfo ButtonFont = FCoreStyle::GetDefaultFontStyle("Roboto", 12);

    TSharedPtr<SHorizontalBox> TitlePanel = SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Center)[
            SAssignNew(TextNameActor, STextBlock)
                .Font(TextFont)
        ];
                   
    TitlePanel->AddSlot()
        .AutoWidth()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Center)[
            SAssignNew(TextNameActor, STextBlock)
                .Font(TextFont)
        ];
                    
    TitlePanel->AddSlot().AutoWidth()[
        SNew(SSpacer).Size(FVector2D(5.0f, 0.0f))
    ];

    Recalculation = SNew(SHorizontalBox);

    Recalculation->AddSlot()
        .AutoWidth()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Center)[
            SAssignNew(MassText, STextBlock)
                .Font(TextFont)
                .Text(FText::FromString(""))
        ];

    Recalculation->AddSlot()
        .AutoWidth()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Center)[
            SNew(SButton)
                .OnClicked_UObject(this, &UInventoryWidget::RecalculationMass)
                .Content()[
                    SNew(STextBlock)
                        .Font(TextFont)
                        .Text(FText::FromString("Recalculation Mass"))
                ]
        ];


    TitlePanel->AddSlot().AutoWidth()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Center)[
            Recalculation.ToSharedRef()
        ];


    TSharedPtr<SBorder> BorderButtons = SNew(SBorder)
        .BorderImage(BrushButtons)
        .Content()[
            TitlePanel.ToSharedRef()
        ];

    InventoryGridBox = NewObject<UScaleBox>(this);

    InventoryGrid = NewObject<UInventoryGrid>(InventoryGridBox);
    InventoryGrid->NoneSlot = USlotNoneWidget::StaticClass();
    InventoryGrid->ItemSlot = USlotItemWidget::StaticClass();

    auto SlotGrid = Cast<UScaleBoxSlot>(InventoryGridBox->AddChild(InventoryGrid));
    SlotGrid->SetHorizontalAlignment(HAlign_Fill);
    SlotGrid->SetVerticalAlignment(VAlign_Fill);

    InventoryListBox = NewObject<UScaleBox>(this);
    InventoryList = NewObject<UInventoryList>(InventoryListBox);
    InventoryList->SizeSlot = 62.0f;
    InventoryList->ItemSlot = USlotItemListWidget::StaticClass();

    auto SlotList = Cast<UScaleBoxSlot>(InventoryListBox->AddChild(InventoryList));
    SlotList->SetHorizontalAlignment(HAlign_Fill);
    SlotList->SetVerticalAlignment(VAlign_Fill);

    MyPanel = SNew(SVerticalBox);

    MyPanel->AddSlot()
    .AutoHeight()
    .VAlign(VAlign_Fill)
    .HAlign(HAlign_Fill)[
        SNew(SBox)
            .Content()[
                BorderButtons.ToSharedRef()
            ]
    ];

    MyPanel->AddSlot()
    .AutoHeight()
    .HAlign(HAlign_Fill)
    .VAlign(VAlign_Fill)[
        SNew(SBorder)
            .BorderImage(BrushInventory)
            .Content()[
                SAssignNew(Switcher, SWidgetSwitcher)
                    + SWidgetSwitcher::Slot()[
                        InventoryGridBox->TakeWidget()
                    ]

                    + SWidgetSwitcher::Slot()[
                        InventoryListBox->TakeWidget()
                    ]
            ]
    ];

    MyPanel->AddSlot()
    .AutoHeight()[
        SNew(SSpacer).Size(FVector2D(0.0f, 10.0f))

    ];

    return MyPanel.ToSharedRef();
}

void UInventoryWidget::RemoveFromParent()
{
    Super::RemoveFromParent();

    Inventory = nullptr;

    switch (Panel)
    {
    case 0:

        if (IsValid(InventoryGrid)) {
            
            InventoryGrid->SetInventory(nullptr);
            InventoryGrid->ClearChildren();
            InventoryGridBox->RemoveChild(InventoryGrid);
            InventoryGridBox->RemoveFromParent();
            InventoryGrid->RemoveFromParent();
        }

        break;

    case 1:
        if (IsValid(InventoryList)) {
            InventoryList->SetInventory(nullptr);
            InventoryList->ClearChildren();
            InventoryListBox->RemoveChild(InventoryList);
            InventoryListBox->RemoveFromParent();
            InventoryList->RemoveFromParent();
        }

        break;
    }
}
