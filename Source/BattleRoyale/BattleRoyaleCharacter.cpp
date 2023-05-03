// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleRoyaleCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SplineComponent.h"
#include "NiagaraComponent.h"
#include "Sound/SoundCue.h"

#include "Survival/PlayerStatsComponent.h"
#include "Survival/LineTrace.h"
#include "Survival/Pickups.h"
#include "Survival/Backpack.h"
#include "Survival/Inventory.h"
#include "Armory/WeaponBase.h"
#include "Armory/ArmoryBase.h"
#include "Armory/Ammo.h"
#include "Armory/GrenadeBase.h"
#include "Projectiles/GrenadeProjectile.h"

#include "UMG/MainWidget.h"
#include "UMG/BackpackWidget.h"
#include "UMG/WeaponWidget.h"
#include "UMG/Damage/DamageWidget.h"
#include "UMG/Damage/KillWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"

#include "BattleRoyalePlayerController.h"
#include "Game/BRGameMode.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameModeBase.h"
#include "Game/BRGameInstance.h"
#include "Engine/GameInstance.h"

#include "Runtime/Core/Public/Delegates/Delegate.h"

//////////////////////////////////////////////////////////////////////////
// ABattleRoyaleCharacter


ABattleRoyaleCharacter::ABattleRoyaleCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	PlayerStatsComponent = CreateDefaultSubobject<UPlayerStatsComponent>("PlayerStatsComponent");
	LineTraceComponent = CreateDefaultSubobject<ULineTrace>("LineTraceComponent");
	InventoryComponent = CreateDefaultSubobject<UInventory>("UInventory");

	bIsSprinting = false;

	bIsUsingMedkit = false;
	bIsUsingWater = false;
	bIsUsingFood = false;

	bIsPunching = false;
	bCanAttack = true;
	bIsFiring = false;
	bIsReloading = false;
	bIsAiming = false;

	bIsWeaponEquipped = false;
	bIsWeaponBeingEquipped = false;
	bIsWeaponBeingUnequipped = false;
	
	bIsHoldingGrenadeLauncher = false;
	bIsGrenadeEquipped = false;
	bIsThrowingGrenade = false;

	bIsTakingDamage = false;
	bIsDead = false;

	bIsCelebrating = false;

	AlivePlayers = 0;
	PlayerScore = 0;
}

void ABattleRoyaleCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Retrieveing and setting game instance
	SetUpGameInstance();

	// Retrieving and setting game mode
	SetUpGameMode();

	// Retrieveing and setting player controller
	SetUpPlayerController();
	SetUpPlayerStatsAtBeginPlay();

	if (IsLocallyControlled() && PlayerMainWidgetClass)
	{
		if (PlayerController)
		{
			SetUpPlayerWidgetAtBeginPlay();
		}
	}
}

void ABattleRoyaleCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bIsCalculatingProjectilePath)
	{
		HandleGrenadeProjectileOnTick();
	}
}

void ABattleRoyaleCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABattleRoyaleCharacter, Weapon);
	DOREPLIFETIME(ABattleRoyaleCharacter, Grenade);
	DOREPLIFETIME(ABattleRoyaleCharacter, Backpack);
	DOREPLIFETIME(ABattleRoyaleCharacter, CurrentPickup);

	DOREPLIFETIME(ABattleRoyaleCharacter, bIsUsingMedkit);
	DOREPLIFETIME(ABattleRoyaleCharacter, bIsUsingWater);
	DOREPLIFETIME(ABattleRoyaleCharacter, bIsUsingFood);

	DOREPLIFETIME_CONDITION(ABattleRoyaleCharacter, bCanAttack, COND_OwnerOnly);
	DOREPLIFETIME(ABattleRoyaleCharacter, bIsPunching);
	DOREPLIFETIME(ABattleRoyaleCharacter, bIsFiring);
	DOREPLIFETIME(ABattleRoyaleCharacter, bIsReloading);
	DOREPLIFETIME(ABattleRoyaleCharacter, bIsAiming);

	DOREPLIFETIME(ABattleRoyaleCharacter, bIsWeaponEquipped);
	DOREPLIFETIME(ABattleRoyaleCharacter, bIsHoldingGrenadeLauncher);
	DOREPLIFETIME(ABattleRoyaleCharacter, bIsGrenadeEquipped);
	DOREPLIFETIME(ABattleRoyaleCharacter, bIsThrowingGrenade);
	
	DOREPLIFETIME(ABattleRoyaleCharacter, bIsTakingDamage);
	DOREPLIFETIME(ABattleRoyaleCharacter, bIsDead);

	DOREPLIFETIME(ABattleRoyaleCharacter, AlivePlayers);
	DOREPLIFETIME_CONDITION(ABattleRoyaleCharacter, PlayerScore, COND_OwnerOnly);

	DOREPLIFETIME(ABattleRoyaleCharacter, bIsCelebrating);
}


//////////////////////////////////////////////////////////////////////////
// Player Basic Setup


bool ABattleRoyaleCharacter::SetUpPlayerController()
{
	if (ABattleRoyalePlayerController* PlayerControllerReference = GetController<ABattleRoyalePlayerController>())
	{
		PlayerController = PlayerControllerReference;
		return true;
	}
	return false;
}

bool ABattleRoyaleCharacter::SetUpGameMode()
{
	GM = Cast<ABRGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		GM->OnPlayersNumUpdated.AddDynamic(this, &ABattleRoyaleCharacter::OnPlayersNumUpdateReceived);
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Warning, TEXT("Players Number : %d"), GM->GetNumPlayers());
#endif
		return true;
	}
	
	else
	{
		return false;
	}
}

bool ABattleRoyaleCharacter::SetUpGameInstance()
{
	GI = Cast<UBRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI)
		return true;

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	UE_LOG(LogTemp, Warning, TEXT("Failed retrieving game instance!"));
#endif
	return false;
}

void ABattleRoyaleCharacter::SetUpPlayerStatsAtBeginPlay()
{
	// Create and pause stamina degeneration timer, unpauses when player starts sprinting
	GetWorld()->GetTimerManager().SetTimer(StaminaDegTimer, this, &ABattleRoyaleCharacter::HandleSprinting, 1.0f, true);
	GetWorld()->GetTimerManager().PauseTimer(StaminaDegTimer);
}

void ABattleRoyaleCharacter::SetUpPlayerWidgetAtBeginPlay()
{
	PlayerMainWidget = CreateWidget<UMainWidget>(PlayerController, PlayerMainWidgetClass);

	if (PlayerMainWidget)
	{
		PlayerMainWidget->AddToPlayerScreen();

		PlayerMainWidget->OnPickupUseDelegate.AddDynamic(this, &ABattleRoyaleCharacter::UsePickup);
		PlayerMainWidget->OnGrenadeUseDelegate.AddDynamic(this, &ABattleRoyaleCharacter::ThrowGrenade);
		PlayerMainWidget->BackpackWidget->OnPickupToDrop.AddDynamic(this, &ABattleRoyaleCharacter::HandleDroppingPickup);
		PlayerMainWidget->WeaponWidget->OnPunchRequested.AddDynamic(this, &ABattleRoyaleCharacter::Punch);
		PlayerMainWidget->WeaponWidget->OnWeaponReloadRequested.AddDynamic(this, &ABattleRoyaleCharacter::WeaponReload);
		PlayerMainWidget->WeaponWidget->OnWeaponDropRequested.AddDynamic(this, &ABattleRoyaleCharacter::DropWeapon);
	}
}

void ABattleRoyaleCharacter::SetPlayerName(const FString& Name)
{
	PlayerName = FName(Name);
}

void ABattleRoyaleCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ABattleRoyaleCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ABattleRoyaleCharacter::StopSprinting);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABattleRoyaleCharacter::StartCrouching);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ABattleRoyaleCharacter::StopCrouching);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABattleRoyaleCharacter::SetIsAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABattleRoyaleCharacter::SetIsNotAiming);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ABattleRoyaleCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ABattleRoyaleCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ABattleRoyaleCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ABattleRoyaleCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("GrenadeVelocity", this, &ABattleRoyaleCharacter::SetGrenadeProjectileVelocity);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ABattleRoyaleCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ABattleRoyaleCharacter::TouchStopped);

	// handle interacting with pickups
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ABattleRoyaleCharacter::Interact);
	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &ABattleRoyaleCharacter::ThrowGrenade);

	// handle interacting with players
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ABattleRoyaleCharacter::Attack);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &ABattleRoyaleCharacter::LaunchGrenadeProjectile);

	PlayerInputComponent->BindAction("Celebrate", IE_Pressed, this, &ABattleRoyaleCharacter::Celebrate);
}


//////////////////////////////////////////////////////////////////////////
// Player Movement


void ABattleRoyaleCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		if (!bIsSprinting && !GetCharacterMovement()->IsCrouching())
			Value *= 0.5f;

		AddMovementInput(Direction, Value);
	}
}

void ABattleRoyaleCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		if (!bIsSprinting && !GetCharacterMovement()->IsCrouching())
			Value *= 0.5f;

		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ABattleRoyaleCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ABattleRoyaleCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ABattleRoyaleCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ABattleRoyaleCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

/* Sprint --> Stamina is decreasing */
void ABattleRoyaleCharacter::Sprint()
{
	if (PlayerStatsComponent->GetStamina() > 20.0f)
	{
		ControlSprintingAffectedByStamina(true);
	}

	else
	{
		ControlSprintingAffectedByStamina(false);
	}
}

/* Stop sprinting --> Stamina is regenerating */
void ABattleRoyaleCharacter::StopSprinting()
{
	bIsSprinting = false;
	PlayerStatsComponent->ControlStaminaTimer(false);
}

void ABattleRoyaleCharacter::HandleSprinting()
{
	if (bIsSprinting)
	{
		PlayerStatsComponent->LowerStamina(1.0f);
	}
}

void ABattleRoyaleCharacter::StartCrouching()
{
	if (!GetCharacterMovement()->IsCrouching() && !GetCharacterMovement()->IsFalling())
	{
		if (bIsSprinting)
			StopSprinting();

		GetCharacterMovement()->bWantsToCrouch = true;
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Warning, TEXT("Crouching"));
#endif
		//GetCharacterMovement()->Crouch();
	}
}

void ABattleRoyaleCharacter::StopCrouching()
{
	if (GetCharacterMovement()->IsCrouching())
	{
		GetCharacterMovement()->bWantsToCrouch = false;
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Warning, TEXT("UnCrouching"));
#endif
		//GetCharacterMovement()->UnCrouch(); 
	}
}


//////////////////////////////////////////////////////////////////////////
// Player Stats | Health


void ABattleRoyaleCharacter::ControlSprintingAffectedByStamina(bool CanSprint)
{
		bIsSprinting = CanSprint;
		PlayerStatsComponent->ControlStaminaTimer(CanSprint);

		if (CanSprint)
		{
			GetWorld()->GetTimerManager().UnPauseTimer(StaminaDegTimer);
		}

		else
		{
			GetWorld()->GetTimerManager().PauseTimer(StaminaDegTimer);
		}
	}

UPlayerStatsComponent* ABattleRoyaleCharacter::GetPlayerStatsComponent()
{
	return PlayerStatsComponent;
}

float ABattleRoyaleCharacter::GetPlayerStamina()
{
	return(PlayerStatsComponent->GetStamina());
}

float ABattleRoyaleCharacter::GetPlayerHealthPercentage()
{
	return(PlayerStatsComponent->GetHealth() / 200.0f);
}

float ABattleRoyaleCharacter::GetPlayerHungerPercentage()
{
	return(PlayerStatsComponent->GetHunger() / 100.0f);
}

float ABattleRoyaleCharacter::GetPlayerThirstPercentage()
{
	return(PlayerStatsComponent->GetThirst() / 100.0f);
}

// Test Function
FString ABattleRoyaleCharacter::ReturnPlayerStats()
{
	FString PlayerStats = "Hunger : " + FString::SanitizeFloat(PlayerStatsComponent->GetHunger())
		+ "  Thirst : " + FString::SanitizeFloat(PlayerStatsComponent->GetThirst())
		+ "  Stamina : " + FString::SanitizeFloat(PlayerStatsComponent->GetStamina())
		+ " Health : " + FString::SanitizeFloat(PlayerStatsComponent->GetHealth());

	return PlayerStats;
}


//////////////////////////////////////////////////////////////////////////
// Player Interaction | Inventory | Pickups


UInventory* ABattleRoyaleCharacter::GetInvetoryComponent()
{
	if (InventoryComponent)
	{
		return InventoryComponent;
	}
	return nullptr;
}

ABackpack* ABattleRoyaleCharacter::GetBackpack()
{
	if (Backpack)
	{
		return Backpack;
	}
	return nullptr;
}

void ABattleRoyaleCharacter::UpdateInventoryCapacity(int Value)
{
	InventoryComponent->SetMaxCapacity(Value);
}

void ABattleRoyaleCharacter::OnRep_PickupInteracted()
{
	if (CurrentPickup)
	{
		// to call it on server side as well
		AttachPickupToHand();
	}
	else
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Warning, TEXT("Invalid pickup!"));
#endif
	}
}

void ABattleRoyaleCharacter::AttachPickupToHand()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerAttachPickupToHand();
	}
	
	else if (GetLocalRole() == ROLE_Authority)
	{
		CurrentPickup->SetActorEnableCollision(false);
		CurrentPickup->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("hand_l_socket"));
		CurrentPickup->SetActorHiddenInGame(false);
	}
}

void ABattleRoyaleCharacter::ServerAttachPickupToHand_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		AttachPickupToHand();
	}
}

FVector ABattleRoyaleCharacter::GetHeadLocation()
{
	return(GetMesh()->GetBoneLocation(FName("head")));
}

FVector ABattleRoyaleCharacter::MakeInteractImpactPoint()
{
	FVector CamStart = FollowCamera->GetComponentLocation();
	FVector CamEnd = CamStart + FollowCamera->GetComponentRotation().Vector() * 500.0f;
	return (LineTraceComponent->LineTraceSingle(CamStart, CamEnd).ImpactPoint);
}

void ABattleRoyaleCharacter::Interact()
{
	FVector ImpactPoint = MakeInteractImpactPoint();

	FVector Start = GetHeadLocation();
	FVector End = Start + UKismetMathLibrary::FindLookAtRotation(Start, ImpactPoint).Vector() * 200.0f;
	FHitResult HitResult = LineTraceComponent->LineTraceSingle(Start, End, true);

	if (AActor* Actor = HitResult.GetActor())
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor : %s"), *Actor->GetName());
#endif
		ServerInteract(Start, End);
	}
}

bool ABattleRoyaleCharacter::ServerInteract_Validate(FVector Start, FVector End)
{
	return true;
}

void ABattleRoyaleCharacter::ServerInteract_Implementation(FVector Start, FVector End)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FHitResult HitResult = LineTraceComponent->LineTraceSingle(Start, End, true);

		if (AActor* Actor = HitResult.GetActor())
		{
			if (APickups* Pickup = Cast<APickups>(Actor))
			{
				if (ABackpack* HitBackpack = Cast<ABackpack>(Pickup))
				{
					HandleBackpackInteraction(HitBackpack); 
				}
				// if its a pickup of type : medkit, water, food
				else
				{
					HandlePickupInteraction(Pickup);
				}
			}

			else if (AWeaponBase* HitWeapon = Cast<AWeaponBase>(Actor))
			{
				HandleWeaponInteraction(HitWeapon);
			}

			else if (AAmmo* HitAmmo = Cast<AAmmo>(Actor))
			{
				HandleAmmoInteraction(HitAmmo);
			}

			else if (AGrenadeBase* HitGrenade = Cast<AGrenadeBase>(Actor))
			{
				HandleGrenadeInteraction(HitGrenade);
			}
		}
	}
}

void ABattleRoyaleCharacter::HandleBackpackInteraction(ABackpack* HitBackpack)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		Backpack = HitBackpack;
		OnRep_BackpackInteracted();
		UpdateInventoryCapacity(Backpack->Capacity);
	}
}

void ABattleRoyaleCharacter::HandlePickupInteraction(APickups* HitPickup)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (InventoryComponent->CanAddPickup(HitPickup->GetPickupWeight()))
		{
			InventoryComponent->AddItem(HitPickup);
			FString CurrentCapacity = FString::FromInt(InventoryComponent->GetCapacity());
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			UE_LOG(LogTemp, Warning, TEXT("Current Capacity : %s"), *CurrentCapacity);
#endif

			EPickupType PickupType = HitPickup->GetPickupType();
			int UpdatedItemNum = this->GetInvetoryComponent()->GetCurrentItemCountPerType(HitPickup->GetPickupType());

			ClientUpdatePickups(PickupType, UpdatedItemNum);
		}
	}
}

void ABattleRoyaleCharacter::HandleWeaponInteraction(AWeaponBase* HitWeapon)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		Weapon = HitWeapon;
		Weapon->SetOwner(this);
		OnRep_WeaponInteracted();
		UpdateWeaponWidgetOnInteracted(true, HitWeapon->WeaponType);
	}
}

void ABattleRoyaleCharacter::HandleAmmoInteraction(AAmmo* HitAmmo)
{
	if (Weapon && Weapon->IsAmmoCompatible(HitAmmo->AmmoType))
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Warning, TEXT("Ammo type is compatible"));
#endif
		HitAmmo->IsPickedUp = true;
		Weapon->UpdateTotalBulletCount(true, HitAmmo->BulletCount);
	}
	else
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Warning, TEXT("Ammo type is not compatible!"));
#endif
	}
}

void ABattleRoyaleCharacter::HandleGrenadeInteraction(AGrenadeBase* HitGrenade)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		Grenade = HitGrenade;
		Grenade->SetOwner(this);
		OnRep_GrenadeInteracted();
	}
	//ClientUpdateGrenade(1);
}

void ABattleRoyaleCharacter::OnRep_BackpackInteracted()
{
	if (Backpack)
	{
		Backpack->Attach();
		Backpack->SetActorEnableCollision(false);
		Backpack->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("spine_socket"));
	}
}

void ABattleRoyaleCharacter::ClientUpdatePickups_Implementation(EPickupType PickupType, int Value)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		PlayerMainWidget->UpdatePickups(PickupType, Value);
		PlayerMainWidget->UpdateBackpack(PickupType, Value);
	}
}

bool ABattleRoyaleCharacter::IsNeedingPickup(EPickupType PickupType)
{
	switch (PickupType)
	{
	case EPickupType::EMedkit:
		return (PlayerStatsComponent->GetHealth() != 200.0f);
		break;

	case EPickupType::EFood:
		return (PlayerStatsComponent->GetHunger() != 100.0f);
		break;

	case EPickupType::EWater:
		return (PlayerStatsComponent->GetThirst() != 100.0f);
		break;

	default:
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Warning, TEXT("Error handling pickup parameter passing"));
#endif
		return false;
	}
}

void ABattleRoyaleCharacter::UsePickup(EPickupType PickupType)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerUsePickup(PickupType);
	}

	else if (GetLocalRole() == ROLE_Authority)
	{
		if (IsNeedingPickup(PickupType))
		{
			// Check if player owns a pickup of the requested type to be used
			if (APickups* Pickup = InventoryComponent->FindFirst(PickupType))
			{
				switch (PickupType)
				{
				case EPickupType::EFood:
				{
					bIsUsingFood = true;
					GetWorld()->GetTimerManager().SetTimer(FoodHandle, this, &ABattleRoyaleCharacter::HandleFoodUsing, 2.0f, false);
					break;
				}
				case EPickupType::EWater:
				{
					bIsUsingWater = true;
					GetWorld()->GetTimerManager().SetTimer(WaterHandle, this, &ABattleRoyaleCharacter::HandleWaterUsing, 3.51f, false);
				}
				case EPickupType::EMedkit:
				{
					bIsUsingMedkit = true;
					GetWorld()->GetTimerManager().SetTimer(MedkitHandle, this, &ABattleRoyaleCharacter::HandleMedkitUsing, 1.8f, false);
				}
				}
				// TODO : move after animation is played
				InventoryComponent->DeleteItem(Pickup);
				Pickup->Destroy();
				ClientUpdatePickups(PickupType, InventoryComponent->GetCurrentItemCountPerType(PickupType));
			}
		}	
		else
		{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			UE_LOG(LogTemp, Warning, TEXT("Player doesn't need pickup!"));
#endif
		}
	}
}

bool ABattleRoyaleCharacter::ServerUsePickup_Validate(EPickupType PickupType)
{
	return true;
}

void ABattleRoyaleCharacter::ServerUsePickup_Implementation(EPickupType PickupType)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		UsePickup(PickupType);
	}
}

bool ABattleRoyaleCharacter::GetIsUsingFood()
{
	return bIsUsingFood;
}

bool ABattleRoyaleCharacter::GetIsUsingWater()
{
	return bIsUsingWater;
}

bool ABattleRoyaleCharacter::GetIsUsingMedkit()
{
	return bIsUsingMedkit;
}

void ABattleRoyaleCharacter::HandleFoodUsing()
{
	bIsUsingFood = false;
	PlayerStatsComponent->RegenerateHunger(40.0f);
}

void ABattleRoyaleCharacter::HandleWaterUsing()
{
	bIsUsingWater = false;
	PlayerStatsComponent->RegenerateThirst(20.0f);
}

void ABattleRoyaleCharacter::HandleMedkitUsing()
{
	bIsUsingMedkit = false;
	PlayerStatsComponent->RegenerateHealth(50.0f);
}

void ABattleRoyaleCharacter::HandleDroppingPickup(EPickupType PickupType)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerHandleDroppingPickup(PickupType);
	}

	else
	{
		// double check if item really exists
		if (InventoryComponent->GetCurrentItemCountPerType(PickupType) == 0)
		{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			UE_LOG(LogTemp, Warning, TEXT("Item doesn't even exist!"));
#endif
		}

		// if it exits
		else
		{
			InventoryComponent->DropItem(InventoryComponent->FindFirst(PickupType));
			int i = InventoryComponent->GetCurrentItemCountPerType(PickupType);
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			UE_LOG(LogTemp, Warning, TEXT("Updated count : %d"), i);
#endif
			ClientUpdatePickupsOnDrop(PickupType, i);
		}
	}
}

bool ABattleRoyaleCharacter::ServerHandleDroppingPickup_Validate(EPickupType PickupType)
{
	return true;
}

void ABattleRoyaleCharacter::ServerHandleDroppingPickup_Implementation(EPickupType PickupType)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		HandleDroppingPickup(PickupType);
	}
}

void ABattleRoyaleCharacter::ClientUpdatePickupsOnDrop_Implementation(EPickupType PickupType, int Value)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		PlayerMainWidget->BackpackWidget->UpdateBackpackOnDrop(PickupType, Value);
		PlayerMainWidget->UpdatePickups(PickupType, Value);
	}
}


//////////////////////////////////////////////////////////////////////////
// Player Attacking | Armory


void ABattleRoyaleCharacter::Punch()
{
	if (Weapon)
	{
		if (bIsWeaponEquipped)
		{
			UnEquipWeapon();
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			UE_LOG(LogTemp, Warning, TEXT("Weapon unequipped!"));
#endif
		}
		// TODO : minimize to one if statement
		else
		{
			ServerPunch();
		}
	}
	else
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Warning, TEXT("No weapon equipped!"));
#endif
		ServerPunch();
	}
}

void ABattleRoyaleCharacter::ServerPunch_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if ((Weapon && !bIsWeaponEquipped) || !Weapon)
			bIsPunching = true;
		GetWorld()->GetTimerManager().SetTimer(PunchHandle, this, &ABattleRoyaleCharacter::HandlePunching, 0.8f, false);
	}
}

void ABattleRoyaleCharacter::HandlePunching()
{
	bIsPunching = false;
}

bool ABattleRoyaleCharacter::GetIsPunching()
{
	return bIsPunching;
}

bool ABattleRoyaleCharacter::HasWeapon()
{
	if (Weapon)
	{
		return true;
	}
	else
	{
		return false;
	}
}

AWeaponBase* ABattleRoyaleCharacter::GetPlayerWeapon()
{
	if (Weapon)
	{
		return Weapon;
	}
	return nullptr;
}

bool ABattleRoyaleCharacter::GetIsWeaponEquipped()
{
	return bIsWeaponEquipped;
}

bool ABattleRoyaleCharacter::GetIsFiring()
{
	return bIsFiring;
}

bool ABattleRoyaleCharacter::GetIsReloading()
{
	return bIsReloading;
}

void ABattleRoyaleCharacter::SetIsFiring(bool Value)
{
	bIsFiring = Value;
}

void ABattleRoyaleCharacter::SetIsReloading(bool Value)
{
	bIsReloading = Value;
}

void ABattleRoyaleCharacter::OnRep_WeaponInteracted()
{
	if (Weapon)
	{
		bIsWeaponEquipped = true;
		if (Weapon->WeaponType == EWeaponType::EGL)
		{
			bIsHoldingGrenadeLauncher = true;
		}
		Weapon->SetActorEnableCollision(false);
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("r_hand_socket"));
		
		if (GetLocalRole() < ROLE_Authority)
		{
			SetupWeaponWidget();
		}
	}
}

void ABattleRoyaleCharacter::EquipWeapon_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		bIsWeaponEquipped = true;
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("r_hand_socket"));
		// TODO : add equip animation : call ServerOnWeaponEquipped after animation delay
		/*
		bIsWeaponBeingEquipped = true;
		GetWorld()->GetTimerManager().SetTimer(EquipHandle, this, &ABattleRoyaleCharacter::ServerOnWeaponEquipped, 0.49f, false);
		*/
	}
}

void ABattleRoyaleCharacter::ServerOnWeaponEquipped_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		bIsWeaponBeingEquipped = false;
		bIsWeaponEquipped = true;
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("r_hand_socket"));
	}
}

void ABattleRoyaleCharacter::UnEquipWeapon_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		bIsWeaponEquipped = false;
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("thigh_r_socket"));
		// TODO : add unequip animation : call ServerOnWeaponUnequipped after animation delay
		/*
		bIsWeaponBeingUnequipped = true;
		GetWorld()->GetTimerManager().SetTimer(UnequipHandle, this, &ABattleRoyaleCharacter::ServerOnWeaponUnequipped, 0.49f, false);
		*/
	}
}

void ABattleRoyaleCharacter::ServerOnWeaponUnequipped_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		bIsWeaponBeingUnequipped = false;
		bIsWeaponEquipped = false;
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("thigh_r_socket"));
	}
}

bool ABattleRoyaleCharacter::CanAttack()
{
	// TODO : add automatic reload if total bullet count is > 0

	if (Weapon->GetMagazineBulletCount() > 0)
	{
		return true;
	}
	return false;
}

void ABattleRoyaleCharacter::ServerCanAttack_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CanAttack();
	}
}

FVector ABattleRoyaleCharacter::MakeAttackImpactPoint()
{
	FVector CamStart = FollowCamera->GetComponentLocation();
	FVector CamEnd = CamStart + FollowCamera->GetComponentRotation().Vector() * Weapon->GetRange();
	return (LineTraceComponent->LineTraceSingleByChannel(CamStart, CamEnd).ImpactPoint);
}

void ABattleRoyaleCharacter::Attack()
{
	if (bIsSprinting)
		bIsSprinting = false;

	if (Weapon && bIsWeaponEquipped)
	{
		if (CanAttack())
		{
			// make player face bullet trace, for each type of weapon
			bUseControllerRotationYaw = true;
			GetWorld()->GetTimerManager().SetTimer(CamRotationHandle, this, &ABattleRoyaleCharacter::HandleCamRotation, 0.4f, false);

			// if player is holding a grenade launcher weapon
			if (bIsHoldingGrenadeLauncher)
			{
				HandleGLAttack();
			}

			// if player is holding a base weapon
			else
			{
				FVector ImpactPoint = MakeAttackImpactPoint();
				FHitResult HitResult = Weapon->Fire(ImpactPoint);

				SimulateWeaponFire();

				if (HitResult.GetActor() != this)
				{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
					UE_LOG(LogTemp, Warning, TEXT("Valid Shot!"));
#endif
					ServerAttack(HitResult);
				}
			}
		}
		else
		{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			UE_LOG(LogTemp, Warning, TEXT("Cannot attack!"));
#endif
		}
	}
	else
	{
		ServerPunch();
	}
}

bool ABattleRoyaleCharacter::ServerAttack_Validate(FHitResult HitResult)
{
	return true;
}

void ABattleRoyaleCharacter::ServerAttack_Implementation(FHitResult HitResult)
{
	if (GetLocalRole() == ROLE_Authority && Weapon)
	{
		bIsFiring = true;
		Weapon->UpdateMagazineBulletCount();
		Weapon->UpdateTotalBulletCount(false, 1);

		// Tests
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Warning, TEXT("reloaded! Bullet Count : %d"), Weapon->TotalBulletCount);
		UE_LOG(LogTemp, Warning, TEXT("Magazine Bullet Count : %d"), Weapon->MagazineBulletCount);
#endif

		Weapon->Fire(HitResult);
		GetWorld()->GetTimerManager().SetTimer(FireHandle, this, &ABattleRoyaleCharacter::HandleFire, 0.26666f, false);
	}
}

void ABattleRoyaleCharacter::SimulateWeaponFire()
{
	Weapon->SimulateFireEffect();
	Weapon->PlaySound(Weapon->BulletSound);
	Weapon->PlaySound(Weapon->BulletEndSound);
}

void ABattleRoyaleCharacter::HandleFire()
{
	bIsFiring = false;
	bUseControllerRotationYaw = false;
}

void ABattleRoyaleCharacter::HandleGLAttack()
{
	Weapon->SetupSpline(false);
	ShowGrenadeProjectilePath();
}

void ABattleRoyaleCharacter::HandleCamRotation()
{
	bUseControllerRotationYaw = false;
}

void ABattleRoyaleCharacter::UpdatePlayerRotationPitch()
{
	bUseControllerRotationPitch = true;
}

void ABattleRoyaleCharacter::ServerAim_Implementation(bool IsAiming)
{
	bIsAiming = IsAiming;
	OnRep_SetIsAiming();
}

bool ABattleRoyaleCharacter::GetIsAiming()
{
	return bIsAiming;
}

void ABattleRoyaleCharacter::SetIsAiming()
{
	if (Weapon)
	{
		ServerAim(true);
	}
}

void ABattleRoyaleCharacter::SetIsNotAiming()
{
	if (Weapon)
	{
		ServerAim(false);
	}
}

void ABattleRoyaleCharacter::OnRep_SetIsAiming()
{
	bUseControllerRotationYaw = bIsAiming;
}

void ABattleRoyaleCharacter::WeaponReload()
{
	if (Weapon)
	{
		if (bIsWeaponEquipped)
		{
			if (Weapon->CanReload())
			{
				SimulateWeaponReload();
				ServerWeaponReload();
			}
			else
			{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
				UE_LOG(LogTemp, Warning, TEXT("Cannot reload!"));
#endif
			}
		}
		else
		{
			EquipWeapon();
		}
	}
	else
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Warning, TEXT("You don't even have a weapon!"));
#endif
	}
}

void ABattleRoyaleCharacter::ServerWeaponReload_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		bIsReloading = true;
		GetWorld()->GetTimerManager().SetTimer(ReloadHandle, this, &ABattleRoyaleCharacter::HandleReload, 1.1f, false);
		Weapon->Reload();
	}
}

void ABattleRoyaleCharacter::SimulateWeaponReload()
{
	Weapon->PlaySound(Weapon->ReloadStartSound);
	Weapon->PlaySound(Weapon->ReloadEndSound);
}

void ABattleRoyaleCharacter::HandleReload()
{
	bIsReloading = false;
}

void ABattleRoyaleCharacter::ClientWeaponReload_Implementation()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Weapon->Reload();
	}
}

void ABattleRoyaleCharacter::DropWeapon()
{
	if (Weapon)
	{
		ServerDropWeapon();
	}

	else
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Warning, TEXT("Weapon doesn't even exist!"));
#endif
	}
}

void ABattleRoyaleCharacter::ServerDropWeapon_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (bIsHoldingGrenadeLauncher)
		{
			bIsHoldingGrenadeLauncher = false;
		}

		// move as a function in weapon base
		bIsWeaponEquipped = false;
		//Weapon->DetachWeaponFromCharacter();
		Weapon->MeshComp->SetEnableGravity(true);
		Weapon->MeshComp->SetSimulatePhysics(true);
		Weapon = nullptr;
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Warning, TEXT("Weapon dropped!"));
#endif
	}
}

FString ABattleRoyaleCharacter::ReturnPlayerWeapon()
{
	if (Weapon)
	{
		FString PlayerWeapon = "Weapon : " + Weapon->GetName()
			+ " Range : " + FString::SanitizeFloat(Weapon->GetRange())
			+ " Damage : " + FString::SanitizeFloat(Weapon->GetDefaultDamage());

		return PlayerWeapon;
	}
	else
	{
		return FString("");
	}
}

void ABattleRoyaleCharacter::OnRep_GrenadeInteracted()
{
	if (Grenade)
	{
		Grenade->SetActorEnableCollision(false);
		Grenade->MeshComp->SetHiddenInGame(true);
		Grenade->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("hand_l_socket"));
	}
}

void ABattleRoyaleCharacter::ThrowGrenade()
{
	// TODO : get a reference to already pickedup grenade
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerThrowGrenade();
	}

	else if (GetLocalRole() == ROLE_Authority)
	{
		if (GrenadeClass)
		{
			//bIsThrowingGrenade = true;

			Grenade = GetWorld()->SpawnActor<AGrenadeBase>(GrenadeClass);

			if (Grenade)
			{
				Grenade->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("hand_l_socket"));
				FVector ForwardVector = UKismetMathLibrary::GetForwardVector(GetControlRotation()) * 2500.0f;
				//Grenade->Throw(ForwardVector);
			}

			//GetWorld()->GetTimerManager().SetTimer(GrenadeHandle, this, &ABattleRoyaleCharacter::HandleGrenade, 1.0f, false);

		}
		else
		{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			UE_LOG(LogTemp, Warning, TEXT("Unable to throw grenade"));
#endif
		}
	}
}

void ABattleRoyaleCharacter::ServerThrowGrenade_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		ThrowGrenade();
	}
}

bool ABattleRoyaleCharacter::GetIsThrowingGrenade()
{
	return bIsThrowingGrenade;
}

void ABattleRoyaleCharacter::HandleGrenade()
{
	bIsThrowingGrenade = false;
}

void ABattleRoyaleCharacter::ShowGrenadeProjectilePath()
{
	bIsCalculatingProjectilePath = true;
}

void ABattleRoyaleCharacter::HandleGrenadeProjectileOnTick()
{
	FPredictProjectilePathParams PredictParams;
	PredictParams.StartLocation = Weapon->GetMuzzleLocation();
	PredictParams.LaunchVelocity = Weapon->GetMuzzleRotation().Vector() * Weapon->LaunchVelocity;
	PredictParams.ProjectileRadius = Weapon->ProjectileRadius;
	FPredictProjectilePathResult PathResult;
	bool bHit = UGameplayStatics::PredictProjectilePath(GetWorld(), PredictParams, PathResult);

	TArray<FVector> PointLocations;

	for (auto PathPoint : PathResult.PathData)
	{
		PointLocations.Add(PathPoint.Location);
	}

	Weapon->SplinePath->SetSplinePoints(PointLocations, ESplineCoordinateSpace::World);
}

void ABattleRoyaleCharacter::SetGrenadeProjectileVelocity(float Delta)
{
	if (bIsHoldingGrenadeLauncher)
	{
		Weapon->SetGrenadeProjectileVelocity(Delta);
	}
}

void ABattleRoyaleCharacter::LaunchGrenadeProjectile()
{
	if (bIsHoldingGrenadeLauncher && CanAttack())
	{
		Weapon->TotalBulletCount--;
		Weapon->SetupSpline(true);
		bIsCalculatingProjectilePath = false;
		AGrenadeProjectile* GrenadeProjectile = Weapon->SpawnGrenadeProjectile();
		GetWorld()->GetTimerManager().SetTimer(GrenadeProjectileHandle, this, &ABattleRoyaleCharacter::HandleGrenadeProjectileDestruction, 0.8f, false);
	}
}

void ABattleRoyaleCharacter::HandleGrenadeProjectileDestruction()
{
	Weapon->DestroyGrenadeProjectile();
	ServerGLAttack();
}

void ABattleRoyaleCharacter::ServerGLAttack_Implementation()
{
	if (GetLocalRole() == ROLE_Authority && Weapon)
	{
		Weapon->UpdateMagazineBulletCount();
		Weapon->UpdateTotalBulletCount(false, 1);

		// Apply Damage, TODO : needs optimization
		/*
		TArray<AActor*> OverlappingActors;
		GrenadeProjectile->ProjectileCollision->GetOverlappingActors(OverlappingActors);
		for (AActor* HitActor : OverlappingActors)
		{
			if (HitActor)
			{
				if (ABattleRoyaleCharacter* HitBRCharacter = Cast<ABattleRoyaleCharacter>(HitActor))
				{
					if (HitBRCharacter != this)
					{
						HitBRCharacter->TakeDamage(180.0f, FDamageEvent(), nullptr, this);
					}
				}
			}
		}
		*/
	}
}


//////////////////////////////////////////////////////////////////////////
// Player Reacting


float ABattleRoyaleCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (GetLocalRole() < ROLE_Authority || PlayerStatsComponent->GetHealth() <= 0)
		return 0.0f;

	
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	
	if (ActualDamage > 0.0f)
	{
		PlayerStatsComponent->LowerHealth(ActualDamage);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Warning, TEXT("Current health : %s"), *FString::SanitizeFloat(PlayerStatsComponent->GetHealth()));
#endif
		if (PlayerStatsComponent->GetHealth() <= 0)
		{
			bIsDead = true;
			GetWorld()->GetTimerManager().SetTimer(DieHandle, this, &ABattleRoyaleCharacter::HandleDie, 1.0f, false);
			Die();
		}
		else
		{
			bIsTakingDamage = true;
			GetWorld()->GetTimerManager().SetTimer(TakeDamageHandle, this, &ABattleRoyaleCharacter::HandleTakingDamage, 0.4f, false);
		}
	}

	return ActualDamage;
}

bool ABattleRoyaleCharacter::GetIsTakingDamage()
{
	return bIsTakingDamage;
}

void ABattleRoyaleCharacter::HandleTakingDamage()
{
	bIsTakingDamage = false;
}

bool ABattleRoyaleCharacter::GetIsDead()
{
	return bIsDead;
}

void ABattleRoyaleCharacter::Die()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		InventoryComponent->DropAllInventory();
		MulticastDie();

		// broadcast to game mode
		GM->UpdatePlayers(UGameplayStatics::GetPlayerController(GetWorld(), 0));

		GetWorld()->GetTimerManager().SetTimer(DestroyHandle, this, &ABattleRoyaleCharacter::CallDestroy, 3.0f, false);
	}
}

void ABattleRoyaleCharacter::HandleDie()
{
	bIsDead = false;
	this->GetMesh()->SetAllBodiesSimulatePhysics(true);
}

void ABattleRoyaleCharacter::MulticastDie_Implementation()
{
	// Ragdoll
	GetCapsuleComponent()->DestroyComponent();
	InputComponent->Deactivate();

	if (UCharacterMovementComponent* CharacterMovementComp = Cast<UCharacterMovementComponent>(this->GetCharacterMovement()))
	{
		CharacterMovementComp->DisableMovement();
	}

	this->GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
}

void ABattleRoyaleCharacter::CallDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		Destroy();
	}
}


//////////////////////////////////////////////////////////////////////////
// Player UMG


void ABattleRoyaleCharacter::SetupWeaponWidget()
{
	FString BulletCountString = FString::FromInt(Weapon->GetMagazineBulletCount()) + "/" + FString::FromInt(Weapon->GetMagazineCapacity());
	FText BulletCountText = FText::FromString(BulletCountString);
}

void ABattleRoyaleCharacter::UpdateWeaponWidgetOnInteracted_Implementation(bool Value, EWeaponType WeaponType)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		PlayerMainWidget->WeaponWidget->SetWeaponIsValid(Value);
		
		if (WeaponType == EWeaponType::ESR)
		{
			PlayerMainWidget->WeaponWidget->ScopeButton->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ABattleRoyaleCharacter::ClientDisplayDamage_Implementation(float Value)
{
	if (GetLocalRole() < ROLE_Authority)
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		UE_LOG(LogTemp, Warning, TEXT("Damage displayed! %f"), Value);
#endif
		GetWorld()->GetTimerManager().SetTimer(DamageDisplayHandle, this, &ABattleRoyaleCharacter::HandleDisplayDamage, 0.4f, false);
		PlayerMainWidget->DamageWidget->DamageTextBlock->SetText(FText::FromString(FString::SanitizeFloat(Value)));
		PlayerMainWidget->DamageWidget->DamageTextBlock->SetVisibility(ESlateVisibility::Visible);

		if (Value > 100)
		{
			FSlateColor Color = FSlateColor(FLinearColor::Red);
			PlayerMainWidget->DamageWidget->DamageTextBlock->SetColorAndOpacity(Color);
		}
	}
}

void ABattleRoyaleCharacter::HandleDisplayDamage()
{
	PlayerMainWidget->DamageWidget->DamageTextBlock->SetVisibility(ESlateVisibility::Hidden);
}

void ABattleRoyaleCharacter::ClientDisplayKillScore_Implementation()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		PlayerMainWidget->KillWidget->SetVisibility(ESlateVisibility::Visible);
		GetWorld()->GetTimerManager().SetTimer(KillHandle, this, &ABattleRoyaleCharacter::HandleKill, 1.2f, false);
	}
}

void ABattleRoyaleCharacter::HandleKill()
{
	PlayerMainWidget->KillWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ABattleRoyaleCharacter::ClientUpdateGrenade_Implementation(int Value)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		PlayerMainWidget->UpdateGrenadeTextBlock(FText::FromString("1"));
	}
}


//////////////////////////////////////////////////////////////////////////
// Gameplay


void ABattleRoyaleCharacter::OnPlayersNumUpdateReceived(int32 PlayersNum)
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	UE_LOG(LogTemp, Warning, TEXT("Updated players num : %d"), PlayersNum);
#endif
	AlivePlayers = PlayersNum;
}

void ABattleRoyaleCharacter::UpdatePlayerScore()
{
	PlayerScore++;
}

int32 ABattleRoyaleCharacter::GetAlivePlayersNum()
{
	return AlivePlayers;
}

int ABattleRoyaleCharacter::GetPlayerScore()
{
	return PlayerScore;
}

bool ABattleRoyaleCharacter::GetIsCelebrating()
{
	return bIsCelebrating;
}

void ABattleRoyaleCharacter::Celebrate()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerCelebrate();
	}
	
	else if (GetLocalRole() == ROLE_Authority)
	{
		bIsCelebrating = true;
		GetWorld()->GetTimerManager().SetTimer(CelebrateHandle, this, &ABattleRoyaleCharacter::HandleCelebrating, 4.1f, false);
	}
}

void ABattleRoyaleCharacter::ServerCelebrate_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		Celebrate();
	}
}

void ABattleRoyaleCharacter::HandleCelebrating()
{
	bIsCelebrating = false;
}
