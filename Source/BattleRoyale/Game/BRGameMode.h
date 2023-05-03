// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Runtime/Core/Public/Delegates/Delegate.h"

#include "BRGameMode.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayersNumUpdated, int32, PlayersNum);

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API ABRGameMode : public AGameMode
{
	GENERATED_BODY()
	
public: 
	ABRGameMode(); 

	virtual void PostLogin(class APlayerController* NewPlayer) override;

	UFUNCTION()
		void UpdatePlayers(class APlayerController* PC);

	void Respawn(class AController* Controller);

public:
	
	FOnPlayersNumUpdated OnPlayersNumUpdated;
	//FOnPlayerScored OnPlayerScored;
};
