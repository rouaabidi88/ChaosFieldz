// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "BattleRoyale/Survival/Pickups.h"
#include "Runtime/Core/Public/Delegates/Delegate.h"
#include "PickupWidget.generated.h"


/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UPickupWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	
	virtual bool Initialize() override;

	UPROPERTY()
		class UBackpackWidget* ParentWidget;

	UPROPERTY(BlueprintReadWrite)
		EPickupType PickupType;

	UPROPERTY(BlueprintReadWrite)
		int PickupCount;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* PickupButton;

	UPROPERTY(BlueprintReadWrite)
		class UTexture2D* Icon;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* PickupCountTextBlock;

	UFUNCTION(BlueprintCallable)
		void SetPickupCountTextBlock(int Value); 

	UFUNCTION(BlueprintCallable)
		void AssignIcon(EPickupType PType);

	UFUNCTION(BlueprintCallable)
		void SetUpPickupWidget(EPickupType PType, int Value);

	UFUNCTION()
		void OnPickupButtonClicked();
};
