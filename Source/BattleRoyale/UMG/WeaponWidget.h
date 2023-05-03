// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Core/Public/Delegates/Delegate.h"

#include "WeaponWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPunchRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponReloadRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponDropRequested);
/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UWeaponWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual bool Initialize() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* PunchButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* WeaponButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* DropWeaponButton;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* BulletCountTextBlock;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* ScopeButton;

	UFUNCTION(BlueprintCallable)
		void OnPunchButtonClicked();

	UFUNCTION(BlueprintCallable)
		void OnWeaponButtonClicked();

	UFUNCTION(BlueprintCallable)
		void OnDropWeaponButtonClicked();

	UFUNCTION(BlueprintCallable)
		void SetWeaponIsValid(bool Value); 

	UPROPERTY()
		bool bWeaponIsValid;

public:

	FOnPunchRequested OnPunchRequested;
	FOnWeaponReloadRequested OnWeaponReloadRequested;
	FOnWeaponDropRequested OnWeaponDropRequested;
}; 
