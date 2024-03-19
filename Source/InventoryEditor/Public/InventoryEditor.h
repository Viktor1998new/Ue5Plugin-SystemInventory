//Copyright(c) 2022 - 2024, Viktor.F.P
#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class SDockTab;

class FInventoryEditorModule : public IModuleInterface
{
public:

	TSharedPtr<class FItemAsset_Action> ItemAssetAction;

	TSharedPtr<FTabManager> TabManager;

	TSharedPtr<FTabManager::FLayout> TabManagerLayout;

	TWeakPtr<SDockTab> MainTab;

	class UEditorWidget* InventoryTab;

	class UBrowserAssetsWidget* BrowserAssetsTab;

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void AddMenuEntry(FMenuBuilder& MenuBuilder);
	void RegisterEditor();
	void UpdateRespawnListIfNeeded(TSharedRef<SDockTab> TabBeingClosed);
	void ChangeTabWorld(UWorld* World, EMapChangeType MapChangeType);

	TSharedRef<SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs);
	TSharedRef<SDockTab> OnRegisterBrowserAssetsTab(const FSpawnTabArgs& SpawnTabArgs);
	TSharedRef<SDockTab> OnRegisterInventoryTab(const FSpawnTabArgs& SpawnTabArgs);
};