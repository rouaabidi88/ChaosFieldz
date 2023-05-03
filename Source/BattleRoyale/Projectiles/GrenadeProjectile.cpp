// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

AGrenadeProjectile::AGrenadeProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectileCollision = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	RootComponent = ProjectileCollision;
	ProjectileCollision->SetSphereRadius(10.0f);
	ProjectileCollision->SetCollisionProfileName("BlockAll");

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	MeshComp->SetupAttachment(ProjectileCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovementComponent->InitialSpeed = 1000.0f;
	ProjectileMovementComponent->Velocity = FVector(1, 0, 0);

}

void AGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGrenadeProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGrenadeProjectile::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
}

void AGrenadeProjectile::SimulateGrenadeEffect()
{
	GrenadeEffectComponent = UGameplayStatics::SpawnEmitterAttached(GrenadeEffect, MeshComp, FName("Grenade_socket"));
}