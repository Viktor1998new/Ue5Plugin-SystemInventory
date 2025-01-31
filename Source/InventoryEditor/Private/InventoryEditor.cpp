//Copyright(c) 2022 - 2025, Viktor.F.P

#include "InventoryEditor.h"
#include "CoreMinimal.h"
#include "Widgets/SWindow.h"
#include "UMGEditor/EditorInventoryWidget.h"
#include "UMGEditor/BrowserAssetsWidget.h"
#include "LevelEditor.h"
#include "Editor.h"
#include "InventoryEditorStyle.h"
#include "ItemAsset_Action.h"
#include "Widgets/Docking/SDockTab.h"

IMPLEMENT_MODULE(FInventoryEditorModule, InventoryEditorModule);

static const FName TabInventoryName = "Inventory";
static const FName TabBrowserAssetsName = "BrowserAssets";

#define LOCTEXT_NAMESPACE "InventoryEditor"

void FInventoryEditorModule::StartupModule()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	EAssetTypeCategories::Type CustomAssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("InventoryCategory")), LOCTEXT("InventoryCategory", "Inventory"));

	TSharedRef<FItemAsset_Action> AssetTypeActions = MakeShareable(new FItemAsset_Action());
	AssetTypeActions->AssetCategory = CustomAssetCategory;

	AssetTools.RegisterAssetTypeActions(AssetTypeActions);

	FLevelEditorModule& LevelEditor = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	HandleChangeTabWorld = LevelEditor.OnMapChanged().AddRaw(this, &FInventoryEditorModule::ChangeTabWorld);

	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
	MenuExtender->AddMenuExtension(
		"LevelEditor",
		EExtensionHook::After,
		NULL,
		FMenuExtensionDelegate::CreateRaw(this, &FInventoryEditorModule::AddMenuEntry));
		
	LevelEditor.GetMenuExtensibilityManager()->AddExtender(MenuExtender);

	FInventoryEditorStyle::Get();
}

void FInventoryEditorModule::AddMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection("InventoryMenu", TAttribute<FText>(FText::FromString("Inventory")));
	{
		MenuBuilder.AddMenuEntry(FText::FromString("Editor Inventory"),
			FText::FromString("Open Inventory Editor"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateRaw(this, &FInventoryEditorModule::RegisterEditor)));
	}
	MenuBuilder.EndSection();
}

void FInventoryEditorModule::RegisterEditor() {
	
	if (!TabManager.IsValid()) {
		FGlobalTabmanager::Get()->RegisterNomadTabSpawner("InventoryEditorTabName", FOnSpawnTab::CreateRaw(this, &FInventoryEditorModule::OnSpawnPluginTab))
			.SetDisplayName(LOCTEXT("InventoryEditor", "InventoryEditor"))
			.SetMenuType(ETabSpawnerMenuType::Hidden);
	}

	FGlobalTabmanager::Get()->TryInvokeTab(FTabId("InventoryEditorTabName"));
}

TSharedRef<SDockTab> FInventoryEditorModule::OnRegisterBrowserAssetsTab(const FSpawnTabArgs& SpawnTabArgs) {
	
	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (World)
	{
		if (BrowserAssetsTab)
		{
			BrowserAssetsTab->Rename(nullptr, GetTransientPackage(), REN_DoNotDirty);
			BrowserAssetsTab = nullptr;
		}
		BrowserAssetsTab = CreateWidget<UBrowserAssetsWidget>(World, UBrowserAssetsWidget::StaticClass());
	}

	auto L_Dock = SNew(SDockTab)
		.TabRole(ETabRole::NumRoles)
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				[
					BrowserAssetsTab->TakeWidget()
				]
		];

	L_Dock->SetTabManager(TabManager);

	return L_Dock;
}

TSharedRef<SDockTab> FInventoryEditorModule::OnRegisterInventoryTab(const FSpawnTabArgs& SpawnTabArgs) {
		
	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (World)
	{
		if (InventoryTab)
		{
			InventoryTab->Rename(nullptr, GetTransientPackage(), REN_DoNotDirty);
			InventoryTab = nullptr;
		}
		InventoryTab = CreateWidget<UEditorInventoryWidget>(World, UEditorInventoryWidget::StaticClass());
	}

	auto L_Dock = SNew(SDockTab)
		.TabRole(ETabRole::NumRoles)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			[
				InventoryTab->TakeWidget()
			]
		];

	L_Dock->SetTabManager(TabManager);

	return L_Dock;
}

void FInventoryEditorModule::UpdateRespawnListIfNeeded(TSharedRef<SDockTab> TabBeingClosed)
{
	if (InventoryTab)
	{
		InventoryTab->Rename(nullptr, GetTransientPackage());
		InventoryTab = nullptr;
	}

	if (BrowserAssetsTab)
	{
		BrowserAssetsTab->Rename(nullptr, GetTransientPackage());
		BrowserAssetsTab = nullptr;
	}
}

TSharedRef<SDockTab> FInventoryEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	const TSharedRef<SDockTab> NomadTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab);

	NomadTab->SetOnTabClosed(SDockTab::FOnTabClosedCallback::CreateRaw(this, &FInventoryEditorModule::UpdateRespawnListIfNeeded));
	
	TabManager = FGlobalTabmanager::Get()->NewTabManager(NomadTab);

	TabManagerLayout = FTabManager::NewLayout("InventoryEditorWindow")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(.4f)
				->AddTab(TabInventoryName, ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(.4f)
				->AddTab(TabBrowserAssetsName, ETabState::OpenedTab)
			)
		);
	
	TabManager.Get()->RegisterTabSpawner(TabInventoryName, FOnSpawnTab::CreateRaw(this, &FInventoryEditorModule::OnRegisterInventoryTab))
		.SetDisplayName(LOCTEXT("Inventory", "Inventory"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	TabManager.Get()->RegisterTabSpawner(TabBrowserAssetsName, FOnSpawnTab::CreateRaw(this, &FInventoryEditorModule::OnRegisterBrowserAssetsTab))
		.SetDisplayName(LOCTEXT("BrowserAssets", "BrowserAssets"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	TSharedRef<SWidget> TabContents = TabManager->RestoreFrom(TabManagerLayout.ToSharedRef(), TSharedPtr<SWindow>(),false, EOutputCanBeNullptr::IfNoTabValid).ToSharedRef();

	NomadTab->SetContent(TabContents);
	MainTab = NomadTab;

	return NomadTab;
}


void FInventoryEditorModule::ChangeTabWorld(UWorld* World, EMapChangeType MapChangeType)
{
	if (MapChangeType == EMapChangeType::TearDownWorld)
	{
		if (MainTab.IsValid())
		{
			if (InventoryTab) {
				InventoryTab->RemoveSelect(TArray<UObject*>());
				InventoryTab->Rename(nullptr, GetTransientPackage());
				InventoryTab = nullptr;
			}

			if (BrowserAssetsTab) {
				BrowserAssetsTab->Rename(nullptr, GetTransientPackage());
				BrowserAssetsTab = nullptr;
			}

			MainTab.Pin()->SetContent(SNullWidget::NullWidget);
			MainTab.Pin()->RequestCloseTab();

		}
	}
}

void FInventoryEditorModule::ShutdownModule()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools")) return;
	
	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(ItemAssetAction.ToSharedRef());

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner("InventoryEditorTabName");

	FLevelEditorModule& LevelEditor = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditor.OnMapChanged().Remove(HandleChangeTabWorld);

}

#undef LOCTEXT_NAMESPACE
