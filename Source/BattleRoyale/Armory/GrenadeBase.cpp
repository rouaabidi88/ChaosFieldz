// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"


AGrenadeBase::AGrenadeBase()
{
	bReplicates = true;
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	RootComponent = MeshComp;
}

void AGrenadeBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGrenadeBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AGrenadeBase::Throw(FVector ForwardVector)
{
	MeshComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
	MeshComp->AddImpulse(ForwardVector);
}


