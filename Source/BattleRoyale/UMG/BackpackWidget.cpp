// Fill out your copyright notice in the Description page of Project Settings.


#include "BackpackWidget.h"
#include "Components/Button.h"

#include "BattleRoyale/BattleRoyalePlayerController.h"
#include "BattleRoyale/Survival/Pickups.h"

#include "PickupWidget.h"
#include "Components/WrapBox.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"



bool UBackpackWidget::Initialize()
{
	Super::Initialize();
	
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (ABattleRoyalePlayerController* BRPC = Cast<ABattleRoyalePlayerController>(PlayerController))
		{
			BRPlayerController = BRPC;
		}
	}
	
	UseButton->OnClicked.AddDynamic(this, &UBackpackWidget::OnUseButtonClicked);
	DropButton->OnClicked.AddDynamic(this, &UBackpackWidget::OnDropButtonClicked);
	
	ClickedOnPickupWidget = nullptr;

	return true;
}

void UBackpackWidget::OnUseButtonClicked()
{

}

void UBackpackWidget::OnDropButtonClicked()
{
	if (ClickedOnPickupWidget != nullptr)
	{
		if (OnPickupToDrop.IsBound())
		{
			OnPickupToDrop.Broadcast(ClickedOnPickupWidget->PickupType);
		}
		UE_LOG(LogTemp, Warning, TEXT("Dropped!"));
	}
}

void UBackpackWidget::UpdateBackpackPickups(EPickupType PickupType, int Value)
{
	if (Value == 0)
	{
		// remove pickup widget with that type
		UPickupWidget* PW = FindPickup(PickupType);
		WrapBox->RemoveChild(PW);
	}

	else if (Value == 1)
	{
		// create a new pickup widget with that type
		if (UPickupWidget* PickupWidget = CreateWidget<UPickupWidget>(BRPlayerController, UPickupWidgetClass))
		{
			PickupWidget->SetUpPickupWidget(PickupType, Value);
			PickupWidget->ParentWidget = this;
			Pickups.Add(PickupWidget);

			WrapBox->AddChild(PickupWidget);
		}
	}

	else
	{
		UPickupWidget* PW = FindPickup(PickupType);
		PW->PickupCount = Value;
		PW->SetPickupCountTextBlock(Value);
	}
}

void UBackpackWidget::UpdateBackpackOnDrop(EPickupType PickupType, int Value)
{
	if (Value == 0)
	{
		// delete the pickup widget
		
		WrapBox->RemoveChild(FindPickup(PickupType));
		UPickupWidget* PW = FindPickup(PickupType);
		PW->PickupCount = Value;
		PW->SetUpPickupWidget(PickupType, Value);
		UE_LOG(LogTemp, Warning, TEXT("Removed pickup widget!"));
	}

	else
	{
		UPickupWidget* PW = FindPickup(PickupType);
		PW->SetUpPickupWidget(PickupType, Value);
		UE_LOG(LogTemp, Warning, TEXT("Decreased pickup count"));
	}
}

UPickupWidget* UBackpackWidget::FindPickup(EPickupType PickupType)
{
	// already checked if its valid
	for (UPickupWidget* PickupWidget : Pickups)
	{
		if (PickupWidget->PickupType == PickupType)
		{
			return PickupWidget;
		}
	}
	return nullptr;
}