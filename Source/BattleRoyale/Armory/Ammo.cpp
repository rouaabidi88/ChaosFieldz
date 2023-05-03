// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo.h"
#include "Net/UnrealNetwork.h"

AAmmo::AAmmo()
{
	bReplicates = true;
	ArmoryType = EArmoryType::EAmmo;
	IsPickedUp = false;
}

void AAmmo::BeginPlay()
{
	Super::BeginPlay();
}

void AAmmo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAmmo, IsPickedUp);
}

void AAmmo::OnRep_Pickedup()
{
	MeshComp->SetHiddenInGame(IsPickedUp);
	SetActorEnableCollision(!IsPickedUp);
}
