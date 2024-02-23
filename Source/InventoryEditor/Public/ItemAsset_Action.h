// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ItemActor.h"
#include "AssetTypeActions_Base.h"

class FItemAsset_Action : public FAssetTypeActions_Base
{
public:

	virtual FText GetName() const override {
		return NSLOCTEXT("AssetTypeActions", "ItemAsset_Action", "Item Asset");
	}

	virtual FColor GetTypeColor() const override {
		return FColor(131, 11, 230);
	}

	virtual UClass* GetSupportedClass() const override {
		return UItemAsset::StaticClass();
	}

	virtual uint32 GetCategories() override {
		return AssetCategory;
	}

	EAssetTypeCategories::Type AssetCategory;
};