// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "GameFramework/Character.h"
#include "BattleRoyale/BattleRoyaleCharacter.h"
#include "BattleRoyale/BattleRoyalePlayerController.h"

#include "BattleRoyale/Survival/Pickups.h"
#include "BattleRoyale/Game/BRGameMode.h"

#include "BackpackWidget.h"
#include "Damage/KillWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Kismet/KismetTextLibrary.h"

#include "Runtime/Core/Public/Delegates/Delegate.h"



bool UMainWidget::Initialize()
{
	Super::Initialize();

	World = GetWorld();


	MedkitButton->OnClicked.AddDynamic(this, &UMainWidget::OnMedkitButtonClicked);
	FoodButton->OnClicked.AddDynamic(this, &UMainWidget::OnFoodButtonClicked);
	WaterButton->OnClicked.AddDynamic(this, &UMainWidget::OnWaterButtonClicked);
	BackpackButton->OnClicked.AddDynamic(this, &UMainWidget::OnBackpackButtonClicked);
	GrenadeButton->OnClicked.AddDynamic(this, &UMainWidget::OnGrenadeButtonClicked);

	MedkitTextBlock->SetText(UKismetTextLibrary::Conv_IntToText(0));
	FoodTextBlock->SetText(UKismetTextLibrary::Conv_IntToText(0));
    WaterTextBlock->SetText(UKismetTextLibrary::Conv_IntToText(0));
	
	GrenadeTextBlock->SetText(UKismetTextLibrary::Conv_IntToText(0));

	BackpackWidget->SetVisibility(ESlateVisibility::Hidden);
	KillWidget->SetVisibility(ESlateVisibility::Hidden);
	
	return true;
}

void UMainWidget::OnMedkitButtonClicked()
{
	if (MedkitTextBlock->GetText().ToString() == "0")
	{
		UE_LOG(LogTemp, Warning, TEXT("No Available Medkits to use !"));
	}
	
	else
	{
		if (OnPickupUseDelegate.IsBound())
		{
			OnPickupUseDelegate.Broadcast(EPickupType::EMedkit);
		}
	}
	
}

void UMainWidget::OnFoodButtonClicked()
{
	if (FoodTextBlock->GetText().ToString() == "0")
	{
		UE_LOG(LogTemp, Warning, TEXT("No Available Food to use !"))
	}
	
	else
	{
		if (OnPickupUseDelegate.IsBound())
		{
			OnPickupUseDelegate.Broadcast(EPickupType::EFood);
		}
	}
	
}

void UMainWidget::OnWaterButtonClicked()
{
	if (WaterTextBlock->GetText().ToString() == "0")
	{
		UE_LOG(LogTemp, Warning, TEXT("No Available Water to use !"))
	}
	
	else
	{
		if (OnPickupUseDelegate.IsBound())
		{
			OnPickupUseDelegate.Broadcast(EPickupType::EWater);
		}
	}
	
}

void UMainWidget::OnBackpackButtonClicked()
{
	if (BackpackWidget->Visibility == ESlateVisibility::Visible)
	{
		BackpackWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (BackpackWidget->Visibility == ESlateVisibility::Hidden)
	{
		BackpackWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMainWidget::OnGrenadeButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Grenade Button Clicked!"));
	if (OnGrenadeUseDelegate.IsBound())
	{
		OnGrenadeUseDelegate.Broadcast();
	}
}

void UMainWidget::UpdateBackpack(EPickupType PickupType, int Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Updating backpack!"));
	BackpackWidget->UpdateBackpackPickups(PickupType, Value);
}

void UMainWidget::UpdatePickups(EPickupType PickupType, int Value)
{
	if (PickupType == EPickupType::EMedkit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Medkit!"));
		MedkitTextBlock->SetText(FText::FromString(FString::FromInt(Value)));
	}
	else if (PickupType == EPickupType::EFood)
	{
		UE_LOG(LogTemp, Warning, TEXT("Food!"));
		FoodTextBlock->SetText(FText::FromString(FString::FromInt(Value)));
	}
	else if (PickupType == EPickupType::EWater)
	{
		UE_LOG(LogTemp, Warning, TEXT("Water!"));
		WaterTextBlock->SetText(FText::FromString(FString::FromInt(Value)));
	}
}

void UMainWidget::UpdateAlivePlayersTextBlock(FText Text)
{
	AlivePlayersTextBlock->SetText(Text);
}

void UMainWidget::UpdateKilledPlayersTextBlock(FText Text)
{
	KilledPlayersTextBlock->SetText(Text);
}

void UMainWidget::UpdateGrenadeTextBlock(FText Text)
{
	if (GrenadeTextBlock)
	{
		GrenadeTextBlock->SetText(Text);
	}
}