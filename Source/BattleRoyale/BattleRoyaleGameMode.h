// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Runtime/Core/Public/Delegates/Delegate.h"

#include "BattleRoyaleGameMode.generated.h"


UCLASS(minimalapi)
class ABattleRoyaleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	/*
	ABattleRoyaleGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	/*
	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchIsWaitingToStart() override;
	


public:
	void Respawn(AController* Controller);

	UFUNCTION()
		void UpdatePlayers(class ABattleRoyalePlayerController* PC);

public:
	/*
	UPROPERTY(ReplicatedUsing = OnPlayersNumUpdated)
	int32 PlayersNum;
	

	FOnPlayersNumUpdated OnPlayersNumUpdated;
	*/
};



