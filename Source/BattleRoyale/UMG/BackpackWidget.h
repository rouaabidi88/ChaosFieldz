// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "BattleRoyale/Survival/Pickups.h"
#include "Runtime/Core/Public/Delegates/Delegate.h"

#include "BackpackWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickupToDrop, EPickupType, PickupType);
/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UBackpackWidget : public UUserWidget
{
	GENERATED_BODY()
	
		
public:
	virtual bool Initialize() override;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UPickupWidget> UPickupWidgetClass;

	/* Array of pickups */
	
	UPROPERTY(BlueprintReadWrite)
		TArray<class UPickupWidget*> Pickups;
	

	UPROPERTY(BlueprintReadWrite)
		class UButton* BackpackTypeButton;

	UPROPERTY(BlueprintReadWrite)
		class UButton* HelmetButton;

	UPROPERTY(BlueprintReadWrite)
		class UButton* VestButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UWrapBox* WrapBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UGridPanel* GridPanel;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* UseButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* DropButton;

	UPROPERTY()
		class UPickupWidget* ClickedOnPickupWidget;

public:
	UFUNCTION(BlueprintCallable)
		void OnUseButtonClicked();

	UFUNCTION(BlueprintCallable)
		void OnDropButtonClicked();

	UFUNCTION(BlueprintCallable)
		void UpdateBackpackPickups(EPickupType PickupType, int Value);

	UFUNCTION(BlueprintCallable)
		void UpdateBackpackOnDrop(EPickupType PickupType, int Value);

	UFUNCTION(Blueprintcallable)
		UPickupWidget* FindPickup(EPickupType PickupType);

public:
	FOnPickupToDrop OnPickupToDrop;

protected:
	class ABattleRoyalePlayerController* BRPlayerController;
	
};
