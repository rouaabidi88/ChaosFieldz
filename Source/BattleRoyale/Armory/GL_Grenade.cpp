// Fill out your copyright notice in the Description page of Project Settings.


#include "GL_Grenade.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"


AGL_Grenade::AGL_Grenade()
{
	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
	RootComponent = CapsuleCollision;
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
}

void AGL_Grenade::BeginPlay()
{
	Super::BeginPlay();
}
