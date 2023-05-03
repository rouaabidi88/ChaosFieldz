// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatsComponent.h"
#include "Net/UnrealNetwork.h"
#include "PlayerStatsComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "BattleRoyale/BattleRoyaleCharacter.h"

UPlayerStatsComponent::UPlayerStatsComponent()
{
	Hunger = 100.0f;
	DecrementedHunger = 0.3f;

	Thirst = 100.0f;
	DecrementedThirst = 0.4f;

	Stamina = 100.0f;

	Health = 200.0f;
}

void UPlayerStatsComponent::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated(true);

	// handling hunger and thirst timer only execute on the server side, no need to run the timer on client side
	if (GetOwnerRole() == ROLE_Authority)
	{
		GetWorld()->GetTimerManager().SetTimer(Timerhandle, this, &UPlayerStatsComponent::HandleHungerAndThirst, 3.0f, true);
	}
	GetWorld()->GetTimerManager().SetTimer(StaminaRegTimer, this, &UPlayerStatsComponent::RegenerateStamina, 1.0f, true);
}

void UPlayerStatsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UPlayerStatsComponent, Hunger, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UPlayerStatsComponent, Thirst, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UPlayerStatsComponent, Stamina, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UPlayerStatsComponent, Health, COND_OwnerOnly);
}

void UPlayerStatsComponent::HandleHungerAndThirst()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		LowerHunger(DecrementedHunger);
		LowerThirst(DecrementedThirst);
	}
}

void UPlayerStatsComponent::ServerLowerHunger_Implementation(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		LowerHunger(Value);
	}
}

void UPlayerStatsComponent::ServerLowerThirst_Implementation(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		LowerThirst(Value);
	}
}

void UPlayerStatsComponent::ServerLowerStamina_Implementation(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		LowerStamina(Value);
	}
}

void UPlayerStatsComponent::ServerControlStaminaTimer_Implementation(bool IsSprinting)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		ControlStaminaTimer(IsSprinting);
	}
}

void UPlayerStatsComponent::ServerLowerHealth_Implementation(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		LowerHealth(Value);
	}
}

void UPlayerStatsComponent::ServerRegenerateHunger_Implementation(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		RegenerateHunger(Value);
	}
}

void UPlayerStatsComponent::ServerRegenerateThirst_Implementation(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		RegenerateThirst(Value);
	}
}

void UPlayerStatsComponent::LowerHunger(float Value)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerLowerHunger(Value);
	}

	else
	{
		Hunger -= Value;

		if (ABattleRoyaleCharacter* Character = Cast<ABattleRoyaleCharacter>(GetOwner()))
		{
			if (Character->GetPlayerStatsComponent()->GetHunger() < 20.0f)
			{
				Character->TakeDamage(2.0f, FDamageEvent(), Character->GetController(), Character);
			}
			else if (Character->GetPlayerStatsComponent()->GetHunger() < 10.0f)
			{
				Character->TakeDamage(5.0f, FDamageEvent(), Character->GetController(), Character);
			}
		}
	}
}

void UPlayerStatsComponent::LowerThirst(float Value)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerLowerThirst(Value);
	}

	else
	{
		Thirst -= Value;

		if (ABattleRoyaleCharacter* Character = Cast<ABattleRoyaleCharacter>(GetOwner()))
		{
			if (Character->GetPlayerStatsComponent()->GetThirst() < 20.0f)
			{
				Character->TakeDamage(2.0f, FDamageEvent(), Character->GetController(), Character);
			}
			else if (Character->GetPlayerStatsComponent()->GetThirst() < 10.0f)
			{
				Character->TakeDamage(5.0f, FDamageEvent(), Character->GetController(), Character);
			}
		}
	}
}

void UPlayerStatsComponent::LowerStamina(float Value)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerLowerStamina(Value);
	}

	else
	{
		Stamina -= Value;
	}
}

void UPlayerStatsComponent::LowerHealth(float Value)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerLowerHealth(Value);
	}

	else
	{
		if (Health - Value < 0)
		{
			Health = 0.0f;
		}
		else
		{
			Health -= Value;
		}
	}
}

// Hunger Regeneration
void UPlayerStatsComponent::RegenerateHunger(float Value)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerRegenerateHunger(Value);
	}
	else
	{
		if (Hunger + Value > 100)
		{
			Hunger = 100;
		}
		else
		{
			// start a timer and play an animation of the food being used by the characater and then regenerate it
			Hunger += Value;
		}
	}
}

// Thirst Regeneration
void UPlayerStatsComponent::RegenerateThirst(float Value)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerRegenerateThirst(Value);
	}
	else
	{
		if (Thirst + Value > 100)
		{
			Thirst = 100;
		}
		else
		{
			// start a timerand play an animation of the water being used by the characater and then regenerate it
			Thirst += Value;
		}
	}
}

void UPlayerStatsComponent::RegenerateStamina()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (Stamina >= 100)
			Stamina = 100.0f;
		else
		{
			++Stamina;
		}
	}
}

void UPlayerStatsComponent::RegenerateHealth(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (Health + Value > 200.0f)
		{
			Health = 200.0f;
		}
		else
		{
			Health += Value;
		}
	}
}

float UPlayerStatsComponent::GetHunger()
{
	return Hunger;
}

float UPlayerStatsComponent::GetThirst()
{
	return Thirst;
}

float UPlayerStatsComponent::GetStamina()
{
	return Stamina;
}

float UPlayerStatsComponent::GetHealth()
{
	return Health;
}

// TODO : make it run on the server
void UPlayerStatsComponent::ControlStaminaTimer(bool IsSprinting)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerControlStaminaTimer(IsSprinting);
	}

	else if(GetOwnerRole() == ROLE_Authority)
	{
		if (IsSprinting)
		{
			GetWorld()->GetTimerManager().PauseTimer(StaminaRegTimer);
		}
		else
		{
			GetWorld()->GetTimerManager().UnPauseTimer(StaminaRegTimer);
		}
	}
}







