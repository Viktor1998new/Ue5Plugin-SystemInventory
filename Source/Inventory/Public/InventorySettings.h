//Copyright(c) 2022 - 2025, Viktor.F.P
#pragma once
#include "Inventory.h"
#include "InventorySettings.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EInventoryFlag : uint8
{
    None = 0 UMETA(hidden),

    Position = 0x01 << 0,
    Size     = 0x01 << 1,
    Mass     = 0x01 << 2,
    LimitY   = 0x01 << 3,
    OnlyX    = 0x01 << 4
};

ENUM_CLASS_FLAGS(EInventoryFlag)

UCLASS(config = Engine, defaultconfig)
class INVENTORY_API UInventorySettings : public UObject {

    GENERATED_UCLASS_BODY()

        virtual void PostInitProperties() override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:

    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "InventorySettings", meta = (Bitmask, BitmaskEnum = "EInventoryFlag"))
        int32 InventoryFlags;

    static UInventorySettings* Get() {
        return FInventoryModule::Get().GetSettings();
    }

    bool HasInventoryFlag(EInventoryFlag Contains)
    {
        return EnumHasAnyFlags<EInventoryFlag>((EInventoryFlag)InventoryFlags, Contains);
    }
};