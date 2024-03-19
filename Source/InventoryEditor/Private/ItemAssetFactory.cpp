//Copyright(c) 2022 - 2024, Viktor.F.P

#include "ItemAssetFactory.h"
#include "Inventory/ItemActor.h"
#include "ClassViewerFilter.h"
#include "Kismet2/SClassPickerDialog.h"
#include "Kismet2/KismetEditorUtilities.h"

class FItemAssetParentFilter : public IClassViewerFilter
{
public:
	FItemAssetParentFilter()
		: DisallowedClassFlags(CLASS_None), bDisallowBlueprintBase(false)
	{}

	TSet< const UClass* > AllowedChildrenOfClasses;
	EClassFlags DisallowedClassFlags;
	bool bDisallowBlueprintBase;

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		bool bAllowed = !InClass->HasAnyClassFlags(DisallowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;

		if (bAllowed && bDisallowBlueprintBase)
		{
			if (FKismetEditorUtilities::CanCreateBlueprintOfClass(InClass))
			{
				return false;
			}
		}

		return bAllowed;
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		if (bDisallowBlueprintBase)
		{
			return false;
		}

		return !InUnloadedClassData->HasAnyClassFlags(DisallowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;
	}
};

UItemAssetFactory::UItemAssetFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UItemAsset::StaticClass();
	bCreateNew = true;
}

bool UItemAssetFactory::ConfigureProperties() {

	ItemAssetClass = nullptr;
	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.ExtraPickerCommonClasses.Add(UItemAsset::StaticClass());

	TSharedPtr<FItemAssetParentFilter> Filter = MakeShareable(new FItemAssetParentFilter);
	Options.ClassFilter = Filter;

	Filter->DisallowedClassFlags = CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists;
	Filter->AllowedChildrenOfClasses.Add(UItemAsset::StaticClass());

	const FText TitleText = INVTEXT("Pick Item Asset Class");
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, UItemAsset::StaticClass());
	
	if (bPressedOk)
	{
		ItemAssetClass = ChosenClass;
	}

	return bPressedOk;
}

UObject* UItemAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn){
	
	if (ItemAssetClass != nullptr)
	{
		return NewObject<UItemAsset>(InParent, ItemAssetClass, Name, Flags | RF_Transactional);
	}
	else
	{
		check(Class->IsChildOf(UItemAsset::StaticClass()));
		return NewObject<UItemAsset>(InParent, Class, Name, Flags);
	}
}
#undef LOCTEXT_NAMESPACE