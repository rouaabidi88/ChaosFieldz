// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickups.generated.h"

UENUM(BlueprintType)
enum class EPickupType : uint8
{
	EWater UMETA(DisplayName = "Water"),
	EFood UMETA(DisplayName = "Food"), 
	EMedkit UMETA(DisplayName = "Medkit")
};

UCLASS()
class BATTLEROYALE_API APickups : public AActor
{
	GENERATED_BODY()
	
public:	
	APickups();

	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere)
		class UTexture2D* Icon;

	UPROPERTY(EditAnywhere)
		EPickupType PickupType;

	UPROPERTY(EditAnywhere)
		int Weight;

	UPROPERTY(ReplicatedUsing = OnRep_PickedUp)
		bool IsPickedUp;

public:
	UFUNCTION(BlueprintCallable)
		class UTexture2D* GetPickupIcon();

	UFUNCTION(BlueprintCallable)
		EPickupType GetPickupType();

	UFUNCTION(BlueprintCallable)
		int GetPickupWeight();

	UFUNCTION()
		void OnRep_PickedUp();

	void InInventory(bool In);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastsDestroyActor();
	void MulticastsDestroyActor_Implementation();
};
