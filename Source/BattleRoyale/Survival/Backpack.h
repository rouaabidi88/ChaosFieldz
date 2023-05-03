// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups.h"
#include "Backpack.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API ABackpack : public APickups
{
	GENERATED_BODY()
	
public:
	ABackpack();


public:
	UPROPERTY(EditAnywhere)
		int Capacity;

public:
	UFUNCTION()
		void Attach();
};
