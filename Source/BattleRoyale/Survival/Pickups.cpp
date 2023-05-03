// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups.h"
#include "Components/StaticMeshComponent.h"
#include "PlayerStatsComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Texture2D.h"

APickups::APickups()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	Icon = CreateDefaultSubobject<UTexture2D>("IconTexture");

	RootComponent = MeshComp;
	bReplicates = true;
	IsPickedUp = false;
	SetReplicateMovement(true);
}

void APickups::BeginPlay()
{
	Super::BeginPlay();
}

void APickups::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickups, IsPickedUp);
}

UTexture2D* APickups::GetPickupIcon()
{
	if (Icon)
		return Icon;
	return nullptr;
}

EPickupType APickups::GetPickupType()
{
	return PickupType;
}

int APickups::GetPickupWeight()
{
	if (PickupType == EPickupType::EMedkit)
	{
		return 10;
	}
	else if (PickupType == EPickupType::EFood)
	{
		return 5;
	}
	else if (PickupType == EPickupType::EWater)
	{
		return 8;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid pickup type")); 
		return 0;
	}
}

void APickups::OnRep_PickedUp()
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	UE_LOG(LogTemp, Warning, TEXT("Item Picked Up"));
#endif

	this->MeshComp->SetHiddenInGame(IsPickedUp);
	this->SetActorEnableCollision(!IsPickedUp);
}

void APickups::InInventory(bool In)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		IsPickedUp = In;
		OnRep_PickedUp();
	}
}

void APickups::MulticastsDestroyActor_Implementation()
{
	Destroy();
}


