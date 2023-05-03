// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Canvas.h"
#include "Engine/DataTable.h"
#include "ArmoryBase.h"
#include "Ammo.h"
#include "WeaponBase.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EAR UMETA(DisplayName = "AssaultRifle"),
	ESG UMETA(DisplayName = "Shotgun"),
	ESR UMETA(DisplayName = "SniperRifle"),
	EHG UMETA(DisplayName = "Handgun"),
	EGL UMETA(DisplayName = "GrenadeLauncher")
};

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		USkeletalMesh* WeaponMesh; 

	UPROPERTY(EditAnywhere)
		FString WeaponName;

	UPROPERTY(EditAnywhere)
		EWeaponType WeaponType;

	UPROPERTY(EditAnywhere)
		float Range;

	UPROPERTY(EditAnywhere)
		float DefaultDamage;

	UPROPERTY(EditAnywhere)
		int MagazineCapacity;

	UPROPERTY(EditAnywhere)
		EAmmoType AmmoType;

	UPROPERTY(EditAnywhere)
		float ReloadDuration;

	UPROPERTY(EditAnywhere)
		float DurationBetweenShots;
}; 

UCLASS()
class BATTLEROYALE_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	

	//////////////////////////////////////////////////////////////////////////
	// AWeaponBase


public:	
	AWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void SetUpWeapon(FName WeaponName);


	//////////////////////////////////////////////////////////////////////////
	// Weapon Components | Armory


public:
	
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Owner)
		class ABattleRoyaleCharacter* WeaponOwner;
	
	UPROPERTY(EditAnywhere)
		FName DefaultWeaponName;

	UPROPERTY(EditAnywhere)
		EWeaponType WeaponType;

	UPROPERTY(EditAnywhere)
		class USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly)
		FCanvasIcon WeaponIcon;

	UPROPERTY(EditAnywhere)
		class UDataTable* WeaponDataTable;

	UPROPERTY()
		float Range;

	UPROPERTY()
		float DefaultDamage;

	UPROPERTY(BlueprintReadWrite)
		int MagazineCapacity;

	UPROPERTY(BlueprintReadWrite, Replicated)
		int MagazineBulletCount;

	UPROPERTY(BlueprintReadWrite, Replicated)
		int TotalBulletCount;

	UPROPERTY(EditAnywhere)
		EAmmoType AmmoType;

	UPROPERTY()
		float ReloadDuration;

	UPROPERTY()
		float DurationBetweenShots;

protected:
	class ULineTrace* LineTraceComponent;

public:
	UFUNCTION(BlueprintCallable)
		void UpdateMagazineBulletCount();

	UFUNCTION()
		bool IsAmmoCompatible(EAmmoType AType);

	/** if increase is true, increase ammo count, otherwise decrease ammo count */
	UFUNCTION()
		void UpdateTotalBulletCount(bool Increase, int Value);
	
	
	//////////////////////////////////////////////////////////////////////////
	// Weapon Attacking

public:
	UPROPERTY(Replicated)
		float AppliedDamage;

public:
	UFUNCTION()
		bool HasEnoughAmmo();

	FVector GetMuzzleLocation();

	FRotator GetMuzzleRotation();

	FHitResult Fire(FVector ImpactPoint);

	FHitResult Fire(FHitResult ClientHitResult);

	bool IsValidHitActor(AActor* HitActor); 

	/** Check if client start and end vectors match server start and end vectors */
	bool AreShotsClose(float Client, float Server);

	/** Check if client's hit result matches server's hit result */
	bool IsValidShot(FHitResult ClientHitResult, FHitResult ServerHitResult);

	float CalculatePlayersDistance(ABattleRoyaleCharacter* Player0, ABattleRoyaleCharacter* Player1);

	float CalculateDamage(FName BoneName, float Distance, bool IsPlayerMoving);
	

	//////////////////////////////////////////////////////////////////////////
	// Weapon Reloading
	
	bool CanReload();

	UFUNCTION(BlueprintCallable)
		void Reload();

	
	//////////////////////////////////////////////////////////////////////////
	// Weapon Effects | Sounds


	UPROPERTY(EditAnywhere)
		class USoundCue* BulletSound;

	UPROPERTY(EditAnywhere)
		class USoundCue* BulletEndSound;

	UPROPERTY(EditAnywhere)
		class USoundCue* ReloadStartSound;

	UPROPERTY(EditAnywhere)
		class USoundCue* ReloadEndSound;

	UPROPERTY(EditAnywhere)
		class USoundCue* GrenadeExplosionSound;

	UPROPERTY(EditDefaultsOnly)
		UParticleSystem* FireTrace;

	UPROPERTY(Transient)
		UParticleSystemComponent* FireTraceComponent;

	UPROPERTY(EditDefaultsOnly)
		UParticleSystem* FireEffect;

	UPROPERTY(Transient)
		UParticleSystemComponent* FireEffectComponent;

public:
	UFUNCTION()
		void SimulateFireEffect();

	UFUNCTION()
		void SimulateGrenadeEffect();
		
	UFUNCTION()
		class UAudioComponent* PlaySound(class USoundCue * Sound);


	//////////////////////////////////////////////////////////////////////////
	// Weapon Getters


public:
	UFUNCTION(BlueprintCallable)
		FString GetName();

	UFUNCTION(BlueprintCallable)
		float GetRange();

	UFUNCTION(BlueprintCallable)
		float GetDefaultDamage();

	UFUNCTION(BlueprintCallable)
		int GetMagazineCapacity();

	UFUNCTION(BlueprintCallable)
		int GetMagazineBulletCount();

	UFUNCTION()
		APawn* GetWeaponOwner();

	/* Grenade Launcher Only */

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class USplineComponent* SplinePath;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UNiagaraComponent* PathVisualEffect;

	UPROPERTY()
		float LaunchVelocity;

	UPROPERTY()
		float ProjectileRadius;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class AGrenadeProjectile> GrenadeProjectileClass;

	UFUNCTION()
		void SetupSpline(bool IsVisible);

	UFUNCTION()
		class AGrenadeProjectile* SpawnGrenadeProjectile();
	
	UFUNCTION()
		void SetGrenadeProjectileVelocity(float Delta);

	/* Spawned Grenade */
	AGrenadeProjectile* GrenadeProjectile;

public:
	void DestroyGrenadeProjectile();

};
