//Copyright(c) 2022 - 2024, Viktor.F.P

#include "UMGEditor/BrowserAssetsWidget.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "SlotsWidget.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Layout/SScrollBox.h"

FReply UBrowserAssetsWidget::OoClickUpData()
{
    if (Slots.IsValidIndex(0)) {
        for (auto Element : Slots) {
            ListAsset.ToSharedRef()->RemoveSlot(Element->TakeWidget());

        }
        Slots.Empty();
    }
    GenerationSlots();

    return FReply::Handled();
}

void UBrowserAssetsWidget::GenerationSlots()
{
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    FARFilter Filter;
    Filter.ClassNames.Add(UItemAsset::StaticClass()->GetFName());
    Filter.PackagePaths.Add("/Game");
    Filter.bRecursivePaths = true;
    Filter.bRecursiveClasses = true;

    TArray<FAssetData> Assets;
    AssetRegistryModule.Get().GetAssets(Filter, Assets);

    for (auto& Element : Assets)
    {
        if (UItemAsset* Asset = Cast<UItemAsset>(Element.GetAsset())) {

            USlotAssetWidget* NewSlotAsset = CreateWidget<USlotAssetWidget>(this, USlotAssetWidget::StaticClass());

            ListAsset.ToSharedRef()->AddSlot()
                [NewSlotAsset->TakeWidget()];

            NewSlotAsset->SetAssetItem(Asset);
            Slots.Add(NewSlotAsset);
        }
    }
}

void UBrowserAssetsWidget::NativePreConstruct()
{
    GenerationSlots();
}

TSharedRef<SWidget> UBrowserAssetsWidget::RebuildWidget()
{
    FSlateFontInfo ButtonFont = FCoreStyle::GetDefaultFontStyle("Roboto", 12);

    TSharedPtr<SButton> ButtonUpData = SNew(SButton)
        .OnClicked_UObject(this, &UBrowserAssetsWidget::OoClickUpData)
        .Content()[
            SNew(STextBlock)
                .Font(ButtonFont)
                .Text(FText::FromString("Up Assets"))
        ].HAlign(HAlign_Center).VAlign(VAlign_Center);

    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)[
            SNew(SScrollBox)
                + SScrollBox::Slot()
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Fill)[
                    SAssignNew(ListAsset, SWrapBox)
                        .Orientation(EOrientation::Orient_Horizontal)
                        .InnerSlotPadding(FVector2D(6.f))
                        .UseAllottedWidth(true)
                ]
        ]

        + SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Bottom)[
                ButtonUpData.ToSharedRef()
            ];
}