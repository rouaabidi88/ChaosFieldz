// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrenadeProjectile.generated.h"

UCLASS()
class BATTLEROYALE_API AGrenadeProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AGrenadeProjectile();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* ProjectileCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY()
		float InitialSpeed;

	UPROPERTY(EditDefaultsOnly)
		class UParticleSystem* GrenadeEffect;

	UPROPERTY(Transient)
		class UParticleSystemComponent* GrenadeEffectComponent;

public:
	UFUNCTION()
		void SimulateGrenadeEffect();
};
