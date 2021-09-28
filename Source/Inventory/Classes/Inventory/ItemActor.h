// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameFramework/Actor.h"
#include "ItemActor.generated.h"

USTRUCT(BlueprintType)
struct INVENTORY_API FItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText NameItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* ImageItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FIntPoint SizeSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MassItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool StackItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool NoneData = false;
};

UCLASS(BlueprintType, Blueprintable)
class INVENTORY_API UItemAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<AItemActor> ClassItemActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FItemData SlotItemData;
	 
};

UCLASS(abstract, BlueprintType)
class INVENTORY_API AItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
		UItemAsset* ItemAsset;

	// Sets default values for this actor's properties
	AItemActor();

	UFUNCTION(BlueprintCallable)
		virtual void SetData(const FString &NewData);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "InitData"))
		void ReceiveInitData(const FString& Data);

	UFUNCTION(BlueprintCallable)
		virtual FString GetData();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "GetData"))
		void ReceiveGetData(FString& Data);
};
