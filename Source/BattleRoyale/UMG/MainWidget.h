// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "BattleRoyale/Survival/Pickups.h" 
#include "Runtime/Core/Public/Delegates/Delegate.h"

#include "MainWidget.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickupUse, EPickupType, PickupType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrenadeUse);

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual bool Initialize() override;


public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UCanvasPanel* MainPanel;


	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		int AlivePlayers;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		int KilledPlayers;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* MedkitButton;

	// Number of available medkits
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* MedkitTextBlock;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* FoodButton; 

	// Number of available food
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* FoodTextBlock;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* WaterButton; 

	// Number of available Water
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* WaterTextBlock;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UButton* BackpackButton;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UButton* GrenadeButton;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UProgressBar* HealthBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UProgressBar* HungerBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UProgressBar* ThirstBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UTextBlock* StaminaTextBlock;
    
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UTextBlock* AlivePlayersTextBlock;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UTextBlock* KilledPlayersTextBlock; 

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UTextBlock* GrenadeTextBlock;


	/* Additional */
	UPROPERTY(EditAnywhere)
		class UButton* WalkButton;

	UPROPERTY(EditAnywhere)
		class UButton* RunButton; 

	UPROPERTY(EditAnywhere)
		class UButton* JumpButton;

	UPROPERTY(EditAnywhere)
		class UButton* CrouchButton;

	UPROPERTY(EditAnywhere)
		class UButton* AttackButton;

	/* Backpack */

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UBackpackWidget> BackpackWidgetClass;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UBackpackWidget* BackpackWidget;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UWeaponWidget> WeaponWidgetClass;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UWeaponWidget* WeaponWidget;
		
	/* Damage */
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UDamageWidget* DamageWidget;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UKillWidget* KillWidget;

	/* Buttons Clicked */
public: 

	UFUNCTION()
		void OnMedkitButtonClicked();
	
	UFUNCTION()
		void OnFoodButtonClicked();
	
	UFUNCTION()
		void OnWaterButtonClicked();
	
	UFUNCTION()
		void OnBackpackButtonClicked();
	
	UFUNCTION()
		void OnGrenadeButtonClicked();

	/* Text Block Updates */
public:
	
	UFUNCTION(BlueprintCallable)
		void UpdateBackpack(EPickupType PickupType, int Value);

	// already firing on server side
	UFUNCTION(Blueprintcallable)	
		void UpdatePickups(EPickupType PickupType, int Value);

	UFUNCTION(Blueprintcallable)
		void UpdateAlivePlayersTextBlock(FText Text);

	UFUNCTION(BlueprintCallable)
		void UpdateKilledPlayersTextBlock(FText Text);

	UFUNCTION(BlueprintCallable)
		void UpdateGrenadeTextBlock(FText Text);

protected:
	class UWorld* World;
	class UInventory* PlayerInventory;

public: 
	/* Delegates */
	FOnPickupUse OnPickupUseDelegate;
	FOnGrenadeUse OnGrenadeUseDelegate;

protected:
	class ABattleRoyalePlayerController* BRPlayerController;

};
