//Copyright(c) 2022 - 2025, Viktor.F.P

#pragma once

#include "CoreMinimal.h"
#include "UMG/Public/Blueprint/UserWidget.h"
#include "Input/Reply.h"
#include "EditorWidget.generated.h"

class UInventoryWidget;

UCLASS(NotBlueprintable)
class INVENTORYEDITOR_API UEditorWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	TSharedPtr<class STextBlock> Text;
	TSharedPtr<class SVerticalBox> ListInventory;

	TArray<UObject*> CurrentSelectActors;

	uint8 Panel;

	FDelegateHandle OnSelectHandle;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnSelect(UObject* NewSelect);

	void OnNoneSelect();

	TMap<AActor*,UInventoryWidget*> Inventorys;

	void AddSelect(TArray<UObject*> NewSelectActors);

	void RemoveSelect(TArray<UObject*> NewSelectActors);

	void ReleaseSlateResources(bool bReleaseChildren) override;

	FReply SwitchList();

	FReply SwitchGrid();

	TArray<UObject*> GetSelectedActors();

protected:

	TSharedPtr<class SConstraintCanvas> MyPanel;

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	
	virtual void PostRename(UObject* OldOuter, const FName OldName) override;
};
