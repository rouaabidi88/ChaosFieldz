// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponWidget.h"
#include "Components/Button.h"

bool UWeaponWidget::Initialize()
{
	Super::Initialize();

	PunchButton->OnClicked.AddDynamic(this, &UWeaponWidget::OnPunchButtonClicked);
	WeaponButton->OnClicked.AddDynamic(this, &UWeaponWidget::OnWeaponButtonClicked);
	DropWeaponButton->OnClicked.AddDynamic(this, &UWeaponWidget::OnDropWeaponButtonClicked);

	ScopeButton->SetVisibility(ESlateVisibility::Hidden);
	bWeaponIsValid = false;

	return true;
}

void UWeaponWidget::OnPunchButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Punch button clicked!"));

	if (OnPunchRequested.IsBound())
	{
		OnPunchRequested.Broadcast();
	}
}

void UWeaponWidget::OnWeaponButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Weapon button clicked!"));

	if (OnWeaponReloadRequested.IsBound() && bWeaponIsValid)
	{
		OnWeaponReloadRequested.Broadcast();
	}
}

void UWeaponWidget::OnDropWeaponButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Drop weapon button clicked!"));

	if (OnWeaponDropRequested.IsBound())
	{
		OnWeaponDropRequested.Broadcast();
	}
}

void UWeaponWidget::SetWeaponIsValid(bool Value)
{
	bWeaponIsValid = Value;
}