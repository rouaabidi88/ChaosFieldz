// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArmoryBase.h"
#include "Ammo.generated.h"

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAR_Ammo UMETA(DisplayName = "AR"),
	ESG_Ammo UMETA(DisplayName = "SG"), 
	ESR_Ammo UMETA(DisplayName = "SR"),
	EHG_Ammo UMETA(DisplayName = "HG"), 
	EGL_Ammo UMETA(DisplayName = "GL")
};

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API AAmmo : public AArmoryBase
{
	GENERATED_BODY()
	
public:
	AAmmo(); 

protected:
	virtual void BeginPlay() override;

public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(EditAnywhere)
		EAmmoType AmmoType;

	UPROPERTY(EditAnywhere)
		int BulletCount;

	UPROPERTY(ReplicatedUsing = OnRep_PickedUp)
		bool IsPickedUp;

public:
	UFUNCTION()
		void OnRep_Pickedup();
};
