// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LineTrace.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEROYALE_API ULineTrace : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULineTrace();

protected:
	virtual void BeginPlay() override;
		
public:
	FHitResult LineTraceSingle(FVector Start, FVector End);
	FHitResult LineTraceSingle(FVector Start, FVector End, bool ShowDebugLine);

	FHitResult LineTraceSingleByChannel(FVector Start, FVector End);
	FHitResult LineTraceSingleByChannel(FVector Start, FVector End, bool ShowDebugLine);
};
