// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h"
#include "PlayerStatsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHungerandThirstDropped, bool, IsHungerDropped, bool, IsThirstDropped);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEROYALE_API UPlayerStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerStatsComponent();

protected:
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* Hunger, Thirst, Stamina */
	void HandleHungerAndThirst();

	UFUNCTION(Server, Reliable)
		void ServerLowerHunger(float Value);
	void ServerLowerHunger_Implementation(float Value);

	UFUNCTION(Server, Reliable)
		void ServerLowerThirst(float Value);
	void ServerLowerThirst_Implementation(float Value);

	UFUNCTION(Server, Reliable)
		void ServerLowerStamina(float Value);
	void ServerLowerStamina_Implementation(float Value);

	UFUNCTION(Server, Reliable)
		void ServerControlStaminaTimer(bool IsSprinting);
	void ServerControlStaminaTimer_Implementation(bool IsSprinting);

	UFUNCTION(Server, Reliable)
		void ServerLowerHealth(float Value);
	void ServerLowerHealth_Implementation(float Value);

	UFUNCTION(Server, Reliable)
		void ServerRegenerateHunger(float Value);
	void ServerRegenerateHunger_Implementation(float Value);

	UFUNCTION(Server, Reliable)
		void ServerRegenerateThirst(float Value);
	void ServerRegenerateThirst_Implementation(float Value);

public:

	/* Lowering */
	void LowerHunger(float Value);
	void LowerThirst(float Value);
	void LowerStamina(float Value);
	void LowerHealth(float Value);

	/* Regeneration */
	void RegenerateHunger(float Value);
	void RegenerateThirst(float Value);
	void RegenerateStamina();
	void RegenerateHealth(float Value);

	float GetHunger();
	float GetThirst();
	float GetStamina();
	float GetHealth();

	void ControlStaminaTimer(bool IsSprinting);

protected:
	    UPROPERTY(Replicated)
			float Hunger;
		
		UPROPERTY(EditAnywhere)
			float DecrementedHunger;

	    UPROPERTY(Replicated)
			float Thirst;
		
		UPROPERTY(EditAnywhere)
			float DecrementedThirst;

		UPROPERTY(Replicated)
			float Stamina;

		UPROPERTY(Replicated)
			float Health;

	FTimerHandle Timerhandle;
	/* Measures !Sprinting : regenerate stamina */
	FTimerHandle StaminaRegTimer;

public:

	//FOnHungerandThirstDropped OnHungerandThirstDropped;
};
