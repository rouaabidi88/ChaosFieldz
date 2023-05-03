// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GL_Grenade.generated.h"

UCLASS()
class BATTLEROYALE_API AGL_Grenade : public AActor
{
	GENERATED_BODY()
	
public:	
	AGL_Grenade();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UCapsuleComponent* CapsuleCollision;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class USkeletalMeshComponent* MeshComp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UProjectileMovementComponent* ProjectileMovement;

};
