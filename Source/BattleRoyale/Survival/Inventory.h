// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "Inventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEROYALE_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventory();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(Replicated)
		TArray<class APickups*> Items;

	UPROPERTY(Replicated)
		TArray<class AGrenadeBase*> Grenades;

	/* Percentage */
	UPROPERTY()
		int Capacity;

	int MaxCapacity;

public:
	void AddItem(class APickups* Pickup);
	void DeleteItem(class APickups* Pickup);
	void DropItem(class APickups* Pickup);
	void DropAllInventory();

	UFUNCTION(BlueprintCallable)
		TArray<class APickups*> GetInventoryItems();
	
	UFUNCTION(BlueprintCallable)
		int32 GetCurrentInventoryCount();

	UFUNCTION(BlueprintCallable)
		int GetCurrentItemCountPerType(EPickupType PType);

	UFUNCTION()
		APickups* FindFirst(EPickupType PickupType);

	UFUNCTION()
		bool CanAddPickup(int Value);

	UFUNCTION()
		int GetCapacity();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastPickupItem(class APickups* Item); 
	void MulticastPickupItem_Implementation(class APickups* Item);

	UFUNCTION()
		void UpdateInventoryCapacity(int AddedCapacity);
		
	void SetMaxCapacity(int Value);
};
