//Copyright(c) 2022 - 2025, Viktor.F.P
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Templates/SubclassOf.h"

#include "Factories/Factory.h"
#include "ItemAssetFactory.generated.h"

class UItemAsset;

UCLASS(hidecategories = Object, MinimalAPI)
class UItemAssetFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = ItemAsset)
	TSubclassOf<UItemAsset> ItemAssetClass;

	// UFactory interface
	virtual bool ConfigureProperties() override;

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	// End of UFactory interface
};
