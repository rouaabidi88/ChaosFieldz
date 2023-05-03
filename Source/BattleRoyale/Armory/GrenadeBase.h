// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrenadeBase.generated.h"

UCLASS()
class BATTLEROYALE_API AGrenadeBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AGrenadeBase();

protected:
	virtual void BeginPlay() override;

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(EditAnywhere)
		class USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly)
		UParticleSystem* Grenade_FireEffect;

	UPROPERTY(Transient)
		UParticleSystemComponent* Grenades_FireEffectComponent;

public:
	UFUNCTION()
		void Throw(FVector ForwardVector);
};
