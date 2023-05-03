// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"
#include "Net/UnrealNetwork.h"
#include "Pickups.h"

#include "Components/StaticMeshComponent.h"

UInventory::UInventory()
{
	MaxCapacity = 50;
	Capacity = 0;
}

void UInventory::BeginPlay()
{
	Super::BeginPlay();

	SetIsReplicated(true);
}

void UInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UInventory, Items, COND_OwnerOnly);
}

void UInventory::AddItem(APickups* Pickup)
{
	Items.Add(Pickup);
	Pickup->InInventory(true);

	switch (Pickup->GetPickupType())
	{
	       case EPickupType::EMedkit:
			   UpdateInventoryCapacity(10);
			   break; 

		   case EPickupType::EFood:
			   UpdateInventoryCapacity(5);
			   break;

		   case EPickupType::EWater:
			   UpdateInventoryCapacity(8);
			   break;

		   default:
			   break;
	}
}

void UInventory::DeleteItem(APickups* Pickup)
{
	Items.Remove(Pickup);

	switch (Pickup->GetPickupType())
	{
	case EPickupType::EMedkit:
		UpdateInventoryCapacity(-10);
		break;

	case EPickupType::EFood:
		UpdateInventoryCapacity(-5);
		break;

	case EPickupType::EWater:
		UpdateInventoryCapacity(-8);
		break;

	default:
		break;
	}
}

void UInventory::DropItem(APickups* Pickup)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		FVector Location = GetOwner()->GetActorLocation();
		Location.X += FMath::RandRange(-50.0f, 100.0f);
		Location.Y += FMath::RandRange(-50.0f, 100.0f);
		Pickup->SetActorLocation(Location);

		Items.Remove(Pickup);
		Pickup->InInventory(false);
	}
}

void UInventory::DropAllInventory()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		/*
		for (APickups* Pickup : Items)
		{
			DropItem(Pickup);
			// TODO : add simulate physics on drop
		}
		Items.Empty();
		*/
		for (int i = 0; i < Items.Num() ; i++)
		{
			DropItem(Items[i]);
		}
		Items.Empty();
	}
}

TArray<APickups*> UInventory::GetInventoryItems()
{
	return Items;
}

int32 UInventory::GetCurrentInventoryCount()
{
	return Items.Num() -1;
}

int UInventory::GetCurrentItemCountPerType(EPickupType PType)
{
	int Num = 0;

	if (Items.Num() == 0)
		return 0;

	else
	{
		for (APickups* Pickup : Items)
		{
			if (Pickup->GetPickupType() == PType)
			{
				Num += 1;
			}
		}
		return Num;
	}
}

APickups* UInventory::FindFirst(EPickupType PickupType)
{
	// check if items is empty 

	for (APickups* Pickup : Items)
	{
		if (Pickup->GetPickupType() == PickupType)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Pickup!"));
			return Pickup;
		}
	}

	APickups* NullPickup = CreateDefaultSubobject<APickups>("NullPickup");
	return NullPickup;
}

bool UInventory::CanAddPickup(int Value)
{
	if (Capacity + Value > MaxCapacity)
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Warning, TEXT("No capacity to add item!"));
#endif
		return false;
	}

	else
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Warning, TEXT("No capacity to add item!"));
#endif
		return true;
	}
}

int UInventory::GetCapacity()
{
	return Capacity;
}

void UInventory::MulticastPickupItem_Implementation(APickups* Item)
{
	Item->SetActorEnableCollision(false);
}

void UInventory::UpdateInventoryCapacity(int AddedCapacity)
{
	Capacity += AddedCapacity;
}

void UInventory::SetMaxCapacity(int Value)
{
	MaxCapacity = Value;
}