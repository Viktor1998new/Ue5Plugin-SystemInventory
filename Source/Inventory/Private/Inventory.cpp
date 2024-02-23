//Copyright(c) 2022, Viktor.F.P

#include "Inventory.h"
#include "InventorySettings.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "FInventoryModule"
DEFINE_LOG_CATEGORY(LogInventory)

void FInventoryModule::StartupModule()
{
	ModuleSettings  = GetMutableDefault<UInventorySettings>();

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Inventory",
			LOCTEXT("RuntimeSettingsName", "Inventory"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Inventory plugin"), ModuleSettings);
	}
}

void FInventoryModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Inventory");
	}
}

UInventorySettings* FInventoryModule::GetSettings() const
{
	check(ModuleSettings);
	return ModuleSettings;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FInventoryModule, Inventory)

