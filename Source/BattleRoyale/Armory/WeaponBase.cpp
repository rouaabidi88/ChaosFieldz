// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "BattleRoyale/Survival/LineTrace.h"
#include "BattleRoyale/BattleRoyaleCharacter.h"
#include "Ammo.h"
#include "BattleRoyale/Projectiles/GrenadeProjectile.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Components/SplineComponent.h"
#include "NiagaraComponent.h"
#include "Particles/ParticleSystem.h"


#include "CollisionQueryParams.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

#include "Net/UnrealNetwork.h"



//////////////////////////////////////////////////////////////////////////
// AWeaponBase


// Sets default values
AWeaponBase::AWeaponBase()
{
	bReplicates = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	RootComponent = MeshComp;

	LineTraceComponent = CreateDefaultSubobject<ULineTrace>("LineTraceComponent");

	SplinePath = CreateDefaultSubobject<USplineComponent>("SplinePath");
	PathVisualEffect = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
	SplinePath->SetupAttachment(MeshComp, FName("MuzzleFlash_socket"));
	PathVisualEffect->SetupAttachment(SplinePath);
	PathVisualEffect->SetHiddenInGame(true, true);
	LaunchVelocity = 1000.0f;
	ProjectileRadius = 10.0f;

	Range = 0.0f;
	DefaultDamage = 0.0f;

	DefaultWeaponName = FName("");

	AppliedDamage = 0.0f;
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	if (DefaultWeaponName != FName(""))
	{
		SetUpWeapon(DefaultWeaponName);
	}
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponBase, MagazineBulletCount);
	DOREPLIFETIME(AWeaponBase, TotalBulletCount);
	DOREPLIFETIME(AWeaponBase, AppliedDamage);
}

void AWeaponBase::SetUpWeapon(FName Name)
{
	if (WeaponDataTable)
	{
		static const FString WString = FString("");
		FWeaponData* WeaponData = WeaponDataTable->FindRow<FWeaponData>(Name, WString, true);

		if (WeaponData)
		{
			MeshComp->SetSkeletalMesh(WeaponData->WeaponMesh);
			WeaponType = WeaponData->WeaponType;
			Range = WeaponData->Range;
			DefaultDamage = WeaponData->DefaultDamage;
			MagazineCapacity = WeaponData->MagazineCapacity;
			MagazineBulletCount = MagazineCapacity;
			TotalBulletCount = MagazineCapacity;
			AmmoType = WeaponData->AmmoType;
			ReloadDuration = WeaponData->ReloadDuration;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// Weapon Components | Armory


void AWeaponBase::UpdateMagazineBulletCount()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		MagazineBulletCount = MagazineBulletCount - 1;
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Warning, TEXT("Magazine Bullet Count : %d"), MagazineBulletCount);
#endif
	}
}

bool AWeaponBase::IsAmmoCompatible(EAmmoType AType)
{
	return (this->AmmoType == AType);
}

void AWeaponBase::UpdateTotalBulletCount(bool Increase, int Value)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (Increase)
		{
			TotalBulletCount += Value;
		}
		else
		{
			TotalBulletCount -= Value;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// Weapon Attacking
	

bool AWeaponBase::HasEnoughAmmo()
{
	return(TotalBulletCount > MagazineBulletCount);
}

FVector AWeaponBase::GetMuzzleLocation()
{
	return (MeshComp->GetSocketLocation(FName("MuzzleFlash_socket")));
}

FRotator AWeaponBase::GetMuzzleRotation()
{
	return (MeshComp->GetSocketRotation(FName("MuzzleFlash_socket")));
}

FHitResult AWeaponBase::Fire(FVector ImpactPoint)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		FVector StartLocation = GetMuzzleLocation();
		FVector EndLocation = StartLocation + UKismetMathLibrary::FindLookAtRotation(StartLocation, ImpactPoint).Vector() * Range;

		FHitResult HitResult = LineTraceComponent->LineTraceSingleByChannel(StartLocation, EndLocation, true);
		return HitResult;
	}
	return(FHitResult());
}

FHitResult AWeaponBase::Fire(FHitResult ClientHitResult)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (AActor* Actor = ClientHitResult.GetActor())
		{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			FString HitActorName = Actor->GetName(); 
			UE_LOG(LogTemp, Warning, TEXT("Hit actor name : %s"), *HitActorName);
#endif
			FVector StartLocation = GetMuzzleLocation();
			FRotator StartRotation = GetMuzzleRotation();
			FVector EndLocation = StartLocation + StartRotation.Vector() * Range;
			FHitResult ServerHitResult = LineTraceComponent->LineTraceSingleByChannel(StartLocation, EndLocation, true);

			if (IsValidShot(ClientHitResult, ServerHitResult))
			{
				if (ABattleRoyaleCharacter* Player = Cast<ABattleRoyaleCharacter>(Actor))
				{
					// display damage for weapon owner
					if (ABattleRoyaleCharacter* PlayerOwner = Cast<ABattleRoyaleCharacter>(GetOwner()))
					{
						float Distance = CalculatePlayersDistance(PlayerOwner, Player);
						// TODO : move check to hit on server side
						AppliedDamage = CalculateDamage(ClientHitResult.BoneName, Distance, false);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
						FString D = FString::SanitizeFloat(AppliedDamage);
						UE_LOG(LogTemp, Warning, TEXT("Damage Taken : %s"), *D);
						UE_LOG(LogTemp, Warning, TEXT("Distance : %f"), Distance);
#endif

						PlayerOwner->ClientDisplayDamage(AppliedDamage);
					}

					Player->TakeDamage(AppliedDamage, FDamageEvent(), nullptr, GetOwner());

					if (Player->bIsDead)
					{
						if (ABattleRoyaleCharacter* ScoringPlayer = Cast<ABattleRoyaleCharacter>(GetOwner()))
						{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
							FString OwnerName = ScoringPlayer->GetName();
							UE_LOG(LogTemp, Warning, TEXT("Owner name is : %s"), *OwnerName);
#endif
							ScoringPlayer->UpdatePlayerScore();
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
							UE_LOG(LogTemp, Warning, TEXT("Player score is : %d"), ScoringPlayer->PlayerScore);
#endif
							ScoringPlayer->ClientDisplayKillScore();
						}
					}
				}
			}
		}
	}
	return FHitResult();
}

bool AWeaponBase::IsValidHitActor(AActor* HitActor)
{
	if (HitActor)
	{
		return true;
	}
	return false;
}

bool AWeaponBase::AreShotsClose(float Client, float Server)
{
	return (Client <= Server + 15.0f || Client > Server - 15.0f);
}

bool AWeaponBase::IsValidShot(FHitResult ClientHitResult, FHitResult ServerHitResult)
{
	// TODO : needs optimization
	/*
	if (!IsValidHitActor(ServerHitResult.GetActor()))
		return false;
	
	float ClientStart = ClientHitResult.TraceStart.Size();
	float ClientEnd = ClientHitResult.TraceEnd.Size(); 

	float ServerStart = ServerHitResult.TraceStart.Size();
	float ServerEnd = ServerHitResult.TraceEnd.Size();

	if (AreShotsClose(ClientStart, ServerStart) && AreShotsClose(ClientEnd, ServerEnd))
	{
		UE_LOG(LogTemp, Warning, TEXT("Valid Shot!")); 
		return true;
	}
	*/
	return true;
}

float AWeaponBase::CalculatePlayersDistance(ABattleRoyaleCharacter* Player_0, ABattleRoyaleCharacter* Player_1)
{
	return ((Player_0->GetActorLocation() - Player_1->GetActorLocation()).Size());
}

float AWeaponBase::CalculateDamage(FName BoneName, float Distance, bool IsPlayerMoving)
{
	TArray<FName> MildImpactBones = { "thigh_l", "thigh_r", "upperarm_l", "upperarm_r", "hand_l", "hand_r" };

	if (Distance < Range/2)
	{
		if (BoneName == "head")
		{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			UE_LOG(LogTemp, Warning, TEXT("Headshot!"));
#endif
			return DefaultDamage + 82;
		}
		else if (MildImpactBones.Contains(BoneName))
		{
			return DefaultDamage - 18;
		}
		return DefaultDamage;
	}
	else if (Distance < Range)
	{
		if (BoneName == "head")
		{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			UE_LOG(LogTemp, Warning, TEXT("Headshot!"));
#endif
			return DefaultDamage + 34;
		}
		else if (MildImpactBones.Contains(BoneName))
		{
			return DefaultDamage - 22;
		}
		return DefaultDamage;
	}
	else
	{
		return DefaultDamage - 26;
	}
}


//////////////////////////////////////////////////////////////////////////
// Weapon Reloading


bool AWeaponBase::CanReload()
{
	return ((MagazineBulletCount < MagazineCapacity) && HasEnoughAmmo()); 
}

void AWeaponBase::Reload()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (TotalBulletCount < MagazineCapacity)
		{
			MagazineBulletCount = TotalBulletCount;
		}
		else
		{
			MagazineBulletCount = MagazineCapacity;
		}
	}
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	UE_LOG(LogTemp, Warning, TEXT("reloaded! Bullet Count : %d"), TotalBulletCount);
	UE_LOG(LogTemp, Warning, TEXT("Magazine Bullet Count : %d"), MagazineBulletCount);
#endif
}


//////////////////////////////////////////////////////////////////////////
// Weapon Effects | Sounds


void AWeaponBase::SimulateFireEffect()
{
	//FireTraceComponent = UGameplayStatics::SpawnEmitterAttached(FireTrace, MeshComp, FName("MuzzleFlash_socket"));
	FireEffectComponent = UGameplayStatics::SpawnEmitterAttached(FireEffect, MeshComp,FName("MuzzleFlash_socket"));
}

void AWeaponBase::SimulateGrenadeEffect()
{
	if (GrenadeProjectile)
	{
		//FireEffectComponent = UGameplayStatics::SpawnEmitterAttached(FireEffect, GrenadeProjectile->MeshComp, FName("Grenade_socket"));
		FireEffectComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireEffect, GrenadeProjectile->GetActorTransform());
	}
}

UAudioComponent* AWeaponBase::PlaySound(USoundCue* Sound)
{
	UAudioComponent* AudioComp = NULL;
	if (Sound)
	{
		AudioComp = UGameplayStatics::SpawnSoundAttached(Sound, MeshComp, FName("MuzzleFlash_socket"));
	}

	return AudioComp;
}


//////////////////////////////////////////////////////////////////////////
// Weapon Getters


FString AWeaponBase::GetName()
{
	return DefaultWeaponName.ToString();
}

float AWeaponBase::GetRange()
{
	return Range;
}

float AWeaponBase::GetDefaultDamage()
{
	return DefaultDamage;
}

int AWeaponBase::GetMagazineCapacity()
{
	return MagazineCapacity;
}

int AWeaponBase::GetMagazineBulletCount()
{
	return MagazineBulletCount;
}

APawn* AWeaponBase::GetWeaponOwner()
{
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		return Pawn;
	}
	return nullptr;
}

void AWeaponBase::SetupSpline(bool IsVisible)
{
	PathVisualEffect->SetHiddenInGame(IsVisible, true);
}

AGrenadeProjectile* AWeaponBase::SpawnGrenadeProjectile()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//AGrenadeProjectile* GrenadeProjectile = GetWorld()->SpawnActor<AGrenadeProjectile>(GrenadeProjectileClass, GetMuzzleLocation(), GetMuzzleRotation(), SpawnParams);

	FTransform GPTransform;
	GPTransform.SetComponents(FQuat::MakeFromEuler(GetMuzzleRotation().Euler()), GetMuzzleLocation(), FVector(1, 1, 1));
	GrenadeProjectile = GetWorld()->SpawnActorDeferred<AGrenadeProjectile>(GrenadeProjectileClass, GPTransform, this, GetWeaponOwner(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	GrenadeProjectile->InitialSpeed = LaunchVelocity;
	UGameplayStatics::FinishSpawningActor(GrenadeProjectile, GPTransform);
	return GrenadeProjectile;
}

void AWeaponBase::SetGrenadeProjectileVelocity(float Delta)
{
	LaunchVelocity += Delta * 100.0f;
	LaunchVelocity = FMath::Clamp(LaunchVelocity, 1000.0f, 5000.0f);
}

void AWeaponBase::DestroyGrenadeProjectile()
{
	SimulateGrenadeEffect();
	PlaySound(GrenadeExplosionSound);
	GrenadeProjectile->Destroy();
}


