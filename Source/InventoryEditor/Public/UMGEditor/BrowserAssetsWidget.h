//Copyright(c) 2022 - 2024, Viktor.F.P

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Input/Reply.h"
#include "BrowserAssetsWidget.generated.h"

UCLASS(NotBlueprintable)
class INVENTORYEDITOR_API UBrowserAssetsWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	TSharedPtr<class SWrapBox> ListAsset;

	TArray<class USlotAssetWidget*> Slots;

	FReply OoClickUpData();

	void GenerationSlots();

protected:

	virtual void NativePreConstruct() override;

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
};