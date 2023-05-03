// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageWidget.h"
#include "Components/TextBlock.h"

bool UDamageWidget::Initialize()
{
	Super::Initialize();

	DamageTextBlock->SetVisibility(ESlateVisibility::Hidden);
	return true;
}
