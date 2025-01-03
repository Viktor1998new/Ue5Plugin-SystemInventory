//Copyright(c) 2022 - 2025, Viktor.F.P

#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UInventorySettings;

DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All);

class FInventoryModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FInventoryModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FInventoryModule>("Inventory");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("Inventory");
	}

	/** Getter for internal settings object to support runtime configuration changes */
	UInventorySettings* GetSettings() const;


private:
	/** Module settings */
	UInventorySettings* ModuleSettings;
};
