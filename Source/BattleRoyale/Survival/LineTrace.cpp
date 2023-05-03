// Fill out your copyright notice in the Description page of Project Settings.


#include "LineTrace.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
ULineTrace::ULineTrace()
{
}

// Called when the game starts
void ULineTrace::BeginPlay()
{
	Super::BeginPlay();
}

FHitResult ULineTrace::LineTraceSingle(FVector Start, FVector End)
{
	FHitResult HitResult;
	FCollisionObjectQueryParams CollisionObjQueryParams;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(GetOwner());
	
	if (GetWorld()->LineTraceSingleByObjectType(OUT HitResult, Start, End, CollisionObjQueryParams, CollisionQueryParams))
	{
		return HitResult;
	}
	return FHitResult();
}

FHitResult ULineTrace::LineTraceSingle(FVector Start, FVector End, bool ShowDebugLine)
{
	FHitResult HitResult = LineTraceSingle(Start, End);
	if (ShowDebugLine)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 3.0f, 0, 5.0f);
	}
	return HitResult;
}

FHitResult ULineTrace::LineTraceSingleByChannel(FVector Start, FVector End)
{
	FHitResult HitResult;
	FCollisionObjectQueryParams CollisionObjQueryParams;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(GetOwner());

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility))
	{
		return HitResult;
	}
	return FHitResult();
}

FHitResult ULineTrace::LineTraceSingleByChannel(FVector Start, FVector End, bool ShowDebugLine)
{
	FHitResult HitResult = LineTraceSingleByChannel(Start, End);
	if (ShowDebugLine)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 3.0f, 0, 5.0f);
	}
	return HitResult;
}


