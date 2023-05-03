// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "BackpackWidget.h"


bool UPickupWidget::Initialize()
{
	Super::Initialize();

	PickupButton->OnClicked.AddDynamic(this, &UPickupWidget::OnPickupButtonClicked);
	
	return true;
}


void UPickupWidget::SetUpPickupWidget(EPickupType PType, int Value)
{
	this->PickupType = PType;
	this->PickupCount = Value;
	SetPickupCountTextBlock(Value);
	AssignIcon(PType);
}

void UPickupWidget::SetPickupCountTextBlock(int Value)
{
	PickupCountTextBlock->SetText(FText::FromString(FString::FromInt(Value))); 
}

void UPickupWidget::AssignIcon(EPickupType PType)
{
}

void UPickupWidget::OnPickupButtonClicked()
{
	ParentWidget->ClickedOnPickupWidget = this;
	UE_LOG(LogTemp, Warning, TEXT("Passed to parent!"));
}
