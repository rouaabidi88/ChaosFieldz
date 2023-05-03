// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "Survival/Pickups.h"
#include "Runtime/Core/Public/Delegates/Delegate.h"
#include "BattleRoyaleGameMode.h"
#include "BattleRoyaleCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPickupInteracted, EPickupType, PickupType, int, UpdatedNum);

UCLASS(config=Game)
class ABattleRoyaleCharacter : public ACharacter
{
	GENERATED_BODY()


	//////////////////////////////////////////////////////////////////////////
	// ABattleRoyaleCharacter


public:
	/** Constructor */
	ABattleRoyaleCharacter();

	/** Begin play */
	virtual void BeginPlay() override;

	/* Tick */
	virtual void Tick(float DeltaSeconds) override;

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	//////////////////////////////////////////////////////////////////////////
	// Player Basic Setup


public:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
		float TurnRateGamepad;

	/** Player Controller */
	class ABattleRoyalePlayerController* PlayerController;

	/** Game Instance */
	class UBRGameInstance* GI;

	/** Current game mode */
	class ABRGameMode* GM;

	bool SetUpPlayerController();
	bool SetUpGameMode();
	bool SetUpGameInstance();

	void SetUpPlayerStatsAtBeginPlay();
	void SetUpPlayerWidgetAtBeginPlay();

	/** player name */
	UPROPERTY(EditAnywhere)
		FName PlayerName;

	UFUNCTION(BlueprintCallable)
		void SetPlayerName(const FString& Name);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	/** Called for player input setup */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	//////////////////////////////////////////////////////////////////////////
	// Player Movement


protected:
	bool bIsSprinting;
	
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	/** Called for sprinting input */
	void Sprint();

	/** Called for stopping sprinting */
	void StopSprinting();

	/** Handle sprinting and stopping sprint */
	void HandleSprinting();

	/** Called for crounching input */
	void StartCrouching();

	/** Called for stopping crouching */
	void StopCrouching();


	//////////////////////////////////////////////////////////////////////////
	// Player Stats | Health


protected:
	/* Measures Sprinting : degenerates stamina */
	FTimerHandle StaminaDegTimer;

	/** Player stats component */
	class UPlayerStatsComponent* PlayerStatsComponent;

public:
	void ControlSprintingAffectedByStamina(bool CanSprint);

	/** Getter for player stats component */
	UFUNCTION()
		UPlayerStatsComponent* GetPlayerStatsComponent();

	UFUNCTION(BlueprintCallable)
		float GetPlayerStamina();

	UFUNCTION(BlueprintCallable)
		float GetPlayerHealthPercentage();

	UFUNCTION(BlueprintCallable)
		float GetPlayerHungerPercentage();

	UFUNCTION(BlueprintCallable)
		float GetPlayerThirstPercentage();

	/** Test player stats */
	UFUNCTION(BlueprintPure)
		FString ReturnPlayerStats();
	

	//////////////////////////////////////////////////////////////////////////
	// Player Interaction | Inventory | Pickups


protected:
	class UInventory* InventoryComponent;

	UPROPERTY(ReplicatedUsing = OnRep_BackpackInteracted)
		class ABackpack* Backpack;
	
	UPROPERTY(ReplicatedUsing = OnRep_PickupInteracted)
		class APickups* CurrentPickup;

	UPROPERTY(Replicated)
		bool bIsUsingMedkit;

	UPROPERTY(Replicated)
		bool bIsUsingWater;

	UPROPERTY(Replicated)
		bool bIsUsingFood;

	FTimerHandle MedkitHandle;
	FTimerHandle WaterHandle;
	FTimerHandle FoodHandle;

public:
	UFUNCTION(BlueprintCallable)
		class UInventory* GetInvetoryComponent();

	UFUNCTION(BlueprintCallable)
		class ABackpack* GetBackpack();

	UFUNCTION()
		void UpdateInventoryCapacity(int Value);

	UFUNCTION()
		void OnRep_PickupInteracted();

	UFUNCTION()
		void AttachPickupToHand();
	
	UFUNCTION(Server, Reliable)
		void ServerAttachPickupToHand();
	void ServerAttachPickupToHand_Implementation();
	
protected:
	/** Get bone head location */
	FVector GetHeadLocation();

	/** Make impact point with line trace */
	FVector MakeInteractImpactPoint();

	/** Called on interact input */
	void Interact();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerInteract(FVector Start, FVector End);
	bool ServerInteract_Validate(FVector Start, FVector End);
	void ServerInteract_Implementation(FVector Start, FVector End);

	/** Handle interactions */
	void HandleBackpackInteraction(class ABackpack* HitBackpack);
	void HandlePickupInteraction(class APickups* HitPickup);
	void HandleWeaponInteraction(class AWeaponBase* HitWeapon);
	void HandleAmmoInteraction(class AAmmo* HitAmmo);
	void HandleGrenadeInteraction(class AGrenadeBase* HitGrenade);

	UFUNCTION()
		void OnRep_BackpackInteracted();

	UFUNCTION(Client, Reliable)
		void ClientUpdatePickups(EPickupType PickupType, int Value);
	void ClientUpdatePickups_Implementation(EPickupType PickupType, int Value);

	UFUNCTION()
		bool IsNeedingPickup(EPickupType PickupType);

	UFUNCTION()
		void UsePickup(EPickupType PickupType);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerUsePickup(EPickupType PickupType);
	bool ServerUsePickup_Validate(EPickupType PickupType);
	void ServerUsePickup_Implementation(EPickupType PickupType);
	
	UFUNCTION(BlueprintCallable)
		bool GetIsUsingFood();

	UFUNCTION(BlueprintCallable)
		bool GetIsUsingWater();

	UFUNCTION(BlueprintCallable)
		bool GetIsUsingMedkit();

	void HandleFoodUsing();
	void HandleWaterUsing();
	void HandleMedkitUsing();

	UFUNCTION(BlueprintCallable)
		void HandleDroppingPickup(EPickupType PickupType);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerHandleDroppingPickup(EPickupType PickupType);
	bool ServerHandleDroppingPickup_Validate(EPickupType PickupType);
	void ServerHandleDroppingPickup_Implementation(EPickupType PickupType);

	UFUNCTION(Client, Reliable)
		void ClientUpdatePickupsOnDrop(EPickupType PickupType, int Value);
	void ClientUpdatePickupsOnDrop_Implementation(EPickupType PickupType, int Value);


	//////////////////////////////////////////////////////////////////////////
	// Player Attacking | Armory


protected:
	UPROPERTY(Replicated)
		bool bIsPunching;

	UPROPERTY(Replicated)
		bool bIsWeaponBeingEquipped;

	UPROPERTY(Replicated)
		bool bIsWeaponBeingUnequipped;

	UPROPERTY(Replicated)
		bool bIsWeaponEquipped;

	UPROPERTY(Replicated)
		bool bIsHoldingGrenadeLauncher;

	UPROPERTY()
		bool bIsCalculatingProjectilePath;

	UPROPERTY(Replicated)
		bool bIsGrenadeEquipped;

	UPROPERTY(Replicated)
		bool bIsThrowingGrenade;

	UPROPERTY(Replicated)
		bool bIsFiring;
	
	UPROPERTY(Replicated)
		bool bIsReloading;
	
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AWeaponBase> WeaponClass;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponInteracted)
		class AWeaponBase* Weapon;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class AGrenadeBase> GrenadeClass;

	UPROPERTY(ReplicatedUsing = OnRep_GrenadeInteracted)
		class AGrenadeBase* Grenade;

	class ULineTrace* LineTraceComponent;

	UPROPERTY(Replicated)
		bool bCanAttack;

	UPROPERTY(ReplicatedUsing = OnRep_SetIsAiming)
		bool bIsAiming;

	FTimerHandle FireHandle;
	FTimerHandle ReloadHandle;
	FTimerHandle GrenadeHandle;
	FTimerHandle DamageDisplayHandle;

	FTimerHandle EquipHandle;
	FTimerHandle UnequipHandle;

	FTimerHandle CamRotationHandle;

public:
	UFUNCTION(BlueprintCallable)
		void Punch();

	UFUNCTION(Server, Reliable)
		void ServerPunch();
	void ServerPunch_Implementation();

	UFUNCTION()
		void HandlePunching();

	UFUNCTION(BlueprintCallable)
		bool GetIsPunching();

	UFUNCTION(BlueprintCallable)
		bool HasWeapon();

	UFUNCTION(BlueprintCallable)
		class AWeaponBase* GetPlayerWeapon();

	UFUNCTION(BlueprintCallable)
		bool GetIsWeaponEquipped();

	UFUNCTION(BlueprintCallable)
		bool GetIsFiring(); 

	UFUNCTION(BlueprintCallable)
		bool GetIsReloading(); 

	void SetIsFiring(bool Value);
	
	void SetIsReloading(bool Value);

	UFUNCTION()
		void OnRep_WeaponInteracted();

	UFUNCTION(Server, Reliable)
		void EquipWeapon();
	void EquipWeapon_Implementation();

	UFUNCTION(Server, Reliable)
		void ServerOnWeaponEquipped();
	void ServerOnWeaponEquipped_Implementation();

	UFUNCTION(Server, Reliable)
		void UnEquipWeapon();
	void UnEquipWeapon_Implementation();

	UFUNCTION(Server, Reliable)
		void ServerOnWeaponUnequipped();
	void ServerOnWeaponUnequipped_Implementation();

	UFUNCTION()
		bool CanAttack();

	UFUNCTION(Server, Reliable)
		void ServerCanAttack();
	void ServerCanAttack_Implementation();

	FVector MakeAttackImpactPoint();
	
	void Attack();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAttack(FHitResult HitResult);
	bool ServerAttack_Validate(FHitResult HitResult);
	void ServerAttack_Implementation(FHitResult HitResult);

	void SimulateWeaponFire();

	void HandleFire();

	void HandleGLAttack();

	UFUNCTION()
		void HandleCamRotation();

	UFUNCTION(BlueprintCallable)
		void UpdatePlayerRotationPitch();

	UFUNCTION(Server, Reliable)
		void ServerAim(bool IsAiming);
	void ServerAim_Implementation(bool IsAiming);

	UFUNCTION(BlueprintCallable)
		bool GetIsAiming();

	void SetIsAiming();
	void SetIsNotAiming();

	UFUNCTION()
		void OnRep_SetIsAiming();

	UFUNCTION(BlueprintCallable)
		void WeaponReload();

	UFUNCTION(Server, Reliable)
		void ServerWeaponReload();
	void ServerWeaponReload_Implementation();

	void SimulateWeaponReload();

	UFUNCTION()
		void HandleReload();

	UFUNCTION(Client, Reliable)
		void ClientWeaponReload();
	void ClientWeaponReload_Implementation();

	UFUNCTION(BlueprintCallable)
		void DropWeapon();

	UFUNCTION(Server, Reliable)
		void ServerDropWeapon();
	void ServerDropWeapon_Implementation();

	// Tests
	UFUNCTION(BlueprintPure)
		FString ReturnPlayerWeapon();

	/* Grenade */

	UFUNCTION()
		void OnRep_GrenadeInteracted();

	UFUNCTION()
		void ThrowGrenade();

	UFUNCTION(Server, Reliable)
		void ServerThrowGrenade();
	void ServerThrowGrenade_Implementation();

	UFUNCTION(BlueprintCallable)
		bool GetIsThrowingGrenade();

	UFUNCTION()
		void HandleGrenade();

	/* Grenade Launcher */

	FTimerHandle GrenadeProjectileHandle;

	UFUNCTION()
		void ShowGrenadeProjectilePath();

	UFUNCTION()
		void HandleGrenadeProjectileOnTick();

	UFUNCTION()
		void SetGrenadeProjectileVelocity(float Delta);

	UFUNCTION()
		void LaunchGrenadeProjectile();

	UFUNCTION()
		void HandleGrenadeProjectileDestruction();

	UFUNCTION(Server, Reliable)
		void ServerGLAttack();
	void ServerGLAttack_Implementation();


	//////////////////////////////////////////////////////////////////////////
	// Player Reacting
		

public:
	UPROPERTY(Replicated)
		bool bIsTakingDamage;
	
	UPROPERTY(Replicated)
		bool bIsDead;

	FTimerHandle PunchHandle;
	FTimerHandle TakeDamageHandle;
	FTimerHandle DestroyHandle;
	FTimerHandle DieHandle;
	FTimerHandle CelebrateHandle;

public:
	UFUNCTION(BlueprintCallable)
		virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable)
		bool GetIsTakingDamage();

	UFUNCTION()
		void HandleTakingDamage();

	UFUNCTION(BlueprintCallable)
		void Die();

	void HandleDie();

	UFUNCTION(BlueprintCallable)
		bool GetIsDead();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastDie();
	void MulticastDie_Implementation();

	void CallDestroy();


	//////////////////////////////////////////////////////////////////////////
	// Player UMG


public:
	FTimerHandle KillHandle;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UMainWidget> PlayerMainWidgetClass;

	UPROPERTY()
		class UMainWidget* PlayerMainWidget;

public:
	UFUNCTION()
		void SetupWeaponWidget();

	UFUNCTION(Client, Reliable)
		void UpdateWeaponWidgetOnInteracted(bool Value, EWeaponType WeaponType);
	void UpdateWeaponWidgetOnInteracted_Implementation(bool Value, EWeaponType WeaponType);

	UFUNCTION(Client, Reliable)
		void ClientDisplayDamage(float Value);
	void ClientDisplayDamage_Implementation(float Value);
		
	UFUNCTION()
		void HandleDisplayDamage();

	UFUNCTION(Client, Reliable)
		void ClientDisplayKillScore();
	void ClientDisplayKillScore_Implementation();

	UFUNCTION()
		void HandleKill();

	UFUNCTION(Client, Reliable)
		void ClientUpdateGrenade(int Value);
	void ClientUpdateGrenade_Implementation(int Value);


	//////////////////////////////////////////////////////////////////////////
	// Gameplay
	

public:
	UPROPERTY(Replicated)
		bool bIsCelebrating;

	UPROPERTY(Replicated)
		int32 AlivePlayers;

	UPROPERTY(BlueprintReadWrite, Replicated)
		int PlayerScore;

public:
	UFUNCTION()
		void OnPlayersNumUpdateReceived(int32 PlayersNum);

	UFUNCTION()
		void UpdatePlayerScore(); 

	UFUNCTION(BlueprintCallable)
		int32 GetAlivePlayersNum();

	UFUNCTION(BlueprintCallable)
		int GetPlayerScore();

	UFUNCTION(BlueprintCallable)
		bool GetIsCelebrating();

	UFUNCTION()
		void Celebrate();

	UFUNCTION(Server, Reliable)
		void ServerCelebrate();
	void ServerCelebrate_Implementation();

	UFUNCTION()
		void HandleCelebrating();


	//////////////////////////////////////////////////////////////////////////
	// Sessions - Online Subsystem


};

