//Copyright(c) 2022 - 2024, Viktor.F.P

#include "UMGEditor/EditorWidget.h"
#include "UMGEditor/InventoryWidget.h"

#include "InventorySettings.h"
#include "Inventory/InventoryComponent.h"

#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "GameFramework/Actor.h"
#include "Editor/UnrealEdEngine.h"
#include "Engine/Selection.h"

#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Brushes/SlateColorBrush.h"

TSharedRef<SWidget> UEditorWidget::RebuildWidget()
{
    FSlateColorBrush* BrushButtons = new  FSlateColorBrush(FLinearColor(0.0f, 0.0f, 0.0f));
    FSlateColorBrush* MyBrush = new FSlateColorBrush(FLinearColor(0.07f, 0.07f, 0.07f));

    FSlateFontInfo ButtonFont = FCoreStyle::GetDefaultFontStyle("Roboto", 12);

    TSharedPtr<SButton> ButtonList = SNew(SButton)
        .OnClicked_UObject(this, &UEditorWidget::SwitchList)
        .Content()[
            SNew(STextBlock)
                .Font(ButtonFont)
                .Text(FText::FromString("List"))
        ];

    TSharedPtr<SButton> ButtonGrid = SNew(SButton)
        .OnClicked_UObject(this, &UEditorWidget::SwitchGrid)
        .Content()[
            SNew(STextBlock)
                .Font(ButtonFont)
                .Text(FText::FromString("Grid"))
        ];


    TSharedPtr<SHorizontalBox> ButtonsPanel = SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .FillWidth(1.0f)
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Fill)
        [
            ButtonList.ToSharedRef()
        ]

        + SHorizontalBox::Slot()
        .FillWidth(1.0f)
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Fill)
        [
            ButtonGrid.ToSharedRef()
        ];


    TSharedPtr<SBorder> L_Border = SNew(SBorder)
        .BorderImage(MyBrush)
        .Content()[
            SNew(SVerticalBox)
                + SVerticalBox::Slot()[
                    SNew(SVerticalBox)
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .VAlign(VAlign_Fill)
                        .HAlign(HAlign_Fill)
                        [
                            SNew(SBorder)
                                .BorderImage(BrushButtons)
                                .Content()[

                                    ButtonsPanel.ToSharedRef()
                                ].HAlign(HAlign_Right)
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .VAlign(VAlign_Fill)
                        .HAlign(HAlign_Fill)[
                            SNew(SSpacer)
                                .Size(FVector2D(1.0f, 9.0f))
                        ]

                        + SVerticalBox::Slot()
                        .FillHeight(1.0f)
                        .VAlign(VAlign_Fill)
                        .HAlign(HAlign_Fill)[
                            SNew(SScrollBox)
                                + SScrollBox::Slot()
                                .HAlign(HAlign_Fill)
                                .VAlign(VAlign_Fill)[
                                    SAssignNew(ListInventory, SVerticalBox)
                                ]
                        ]
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .VAlign(VAlign_Bottom)
                .HAlign(HAlign_Fill)[
                    SNew(STextBlock)
                        .Font(ButtonFont)
                        .Text(FText::FromString("Drag: LMB, Menu: RMB, Rotate item: Drag + Left Shift"))
                ]
        ];

    MyPanel = SNew(SConstraintCanvas)
        + SConstraintCanvas::Slot()
        .Anchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f))
        .Offset(FMargin(0.0f))
        [
            L_Border.ToSharedRef()
        ];

	return MyPanel.ToSharedRef();
}

void UEditorWidget::PostRename(UObject* OldOuter, const FName OldName)
{
    CurrentSelectActors.Empty();
    ListInventory->ClearChildren();
    Inventorys.Empty();
}

void UEditorWidget::NativeConstruct()
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
 
    if (!EditorEngine) return;
  
    USelection* Selection = EditorEngine->GetSelectedActors();
   
    if (!Selection) return;

    OnSelect(nullptr);

    OnSelectHandle = Selection->SelectionChangedEvent.AddUFunction(this, FName("OnSelect"));
}

void UEditorWidget::NativeDestruct()
{
    CurrentSelectActors.Empty();
    ListInventory->ClearChildren();
    Inventorys.Empty();

    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (!EditorEngine) return;

    USelection* Selection = EditorEngine->GetSelectedActors();
    if (!Selection) return;

    Selection->SelectionChangedEvent.Remove(OnSelectHandle);
}

void UEditorWidget::OnSelect(UObject* NewSelect)
{
    TArray<UObject*> L_Select = GetSelectedActors();

    if (L_Select.Num() == 0 && CurrentSelectActors.Num() == 0)
        return;

    if (L_Select.Num() > CurrentSelectActors.Num()) {
        AddSelect(L_Select);
    }
    else if (L_Select.Num() == CurrentSelectActors.Num()) {
        if (L_Select.Num() == 1 && CurrentSelectActors.Num() == 1) {
            if (L_Select[0] != CurrentSelectActors[0]) {

                if (AActor* SelectActor = Cast<AActor>(CurrentSelectActors[0])) {
                    if (Inventorys.Find(SelectActor)) {
                        UInventoryWidget* InventoryWidget = Inventorys[SelectActor];
                        ListInventory->RemoveSlot(InventoryWidget->TakeWidget());
                        InventoryWidget->RemoveFromParent();
                        Inventorys.Remove(SelectActor);
                        CurrentSelectActors = L_Select;
                    }
                }

                if (AActor* SelecActor = Cast<AActor>(L_Select[0])) {

                    if (UInventoryComponent* NewInventory = Cast<UInventoryComponent>(SelecActor->GetComponentByClass(UInventoryComponent::StaticClass()))) {

                        UInventoryWidget* InventoryWidget = CreateWidget<UInventoryWidget>(this, UInventoryWidget::StaticClass());

                        ListInventory->AddSlot()
                            .AutoHeight()
                            .VAlign(VAlign_Top)
                            .HAlign(HAlign_Fill)
                            .Padding(FMargin(0.0f, 0.0f, 0.0f, 0.0f))
                            .AttachWidget(InventoryWidget->TakeWidget());

                        InventoryWidget->SetInventory(NewInventory, Panel);
                        Inventorys.Add(SelecActor, InventoryWidget);
                        CurrentSelectActors = L_Select;
                    }
                }
            }
        }
    }
    else if (L_Select.Num() < CurrentSelectActors.Num()) {

        RemoveSelect(L_Select);

        if (L_Select.Num() == 1) {

            if (CurrentSelectActors.Find(L_Select[0]) == INDEX_NONE) {

                if (AActor* SelecActor = Cast<AActor>(L_Select[0])) {

                    if (UInventoryComponent* NewInventory = Cast<UInventoryComponent>(SelecActor->GetComponentByClass(UInventoryComponent::StaticClass()))) {

                        UInventoryWidget* InventoryWidget = CreateWidget<UInventoryWidget>(this, UInventoryWidget::StaticClass());

                        ListInventory->AddSlot()
                            .AutoHeight()
                            .VAlign(VAlign_Fill)
                            .HAlign(HAlign_Left)
                            .Padding(FMargin(0.0f, 0.0f, 0.0f, 0.0f))
                            .AttachWidget(InventoryWidget->TakeWidget());

                        InventoryWidget->SetInventory(NewInventory, Panel);
                        Inventorys.Add(SelecActor, InventoryWidget);
                        CurrentSelectActors = L_Select;
                    }
                }
            }
        }
    }
}

void UEditorWidget::OnNoneSelect() {
    OnSelect(nullptr);
}

void UEditorWidget::AddSelect(TArray<UObject*> NewSelectActors)
{
    for (auto& NewSelect : NewSelectActors) {

        if (CurrentSelectActors.Find(NewSelect) == INDEX_NONE) {
            
            if (AActor* SelecActor = Cast<AActor>(NewSelect)) {

                if (UInventoryComponent* NewInventory = Cast<UInventoryComponent>(SelecActor->GetComponentByClass(UInventoryComponent::StaticClass()))) {
                    
                    UInventoryWidget* InventoryWidget = CreateWidget<UInventoryWidget>(this, UInventoryWidget::StaticClass());
                    
                    ListInventory->AddSlot()
                        .AutoHeight()
                        .VAlign(VAlign_Top)
                        .HAlign(HAlign_Fill)
                        .Padding(FMargin(0.0f,0.0f,0.0f,0.0f))
                        .AttachWidget(InventoryWidget->TakeWidget());

                    InventoryWidget->SetInventory(NewInventory, Panel);
                    Inventorys.Add(SelecActor, InventoryWidget);
                }
            }
        }
    }

    CurrentSelectActors = NewSelectActors;
}

void UEditorWidget::RemoveSelect(TArray<UObject*> NewSelectActors)
{
    for (auto& Select : CurrentSelectActors) {

        if (!IsValid(Select))
            continue;

        if (NewSelectActors.Find(Select) == INDEX_NONE) {

            if (AActor* SelectActor = Cast<AActor>(Select)) {
                if (Inventorys.Find(SelectActor)) {
                    UInventoryWidget* InventoryWidget = Inventorys[SelectActor];
                    ListInventory->RemoveSlot(InventoryWidget->TakeWidget());
                    InventoryWidget->RemoveFromParent();
                    Inventorys.Remove(SelectActor);
                }
            }
        }
    }

    CurrentSelectActors = NewSelectActors;
}

void UEditorWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyPanel.Reset();
}

FReply UEditorWidget::SwitchList()
{
    Panel = 1;
    for (auto& Element : Inventorys) {
        Element.Value->SwitchPanel(Panel);
    }

    return FReply::Handled();
}

FReply UEditorWidget::SwitchGrid()
{
    Panel = 0;
    for (auto& Element : Inventorys) {
        Element.Value->SwitchPanel(Panel);
    }

    return FReply::Handled();
}

TArray<UObject*> UEditorWidget::GetSelectedActors()
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (!EditorEngine)
    {
        return TArray<UObject*>();
    }

    USelection* Selection = EditorEngine->GetSelectedActors();
    if (!Selection)
    {
        return TArray<UObject*>();
    }
    TArray<UObject*> SelectedActors;
    Selection->GetSelectedObjects(AActor::StaticClass(), SelectedActors);

    return SelectedActors;
}