// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleRoyaleGameMode.h"
#include "BattleRoyaleCharacter.h"
#include "BattleRoyale/BattleRoyalePlayerController.h"

#include "UObject/ConstructorHelpers.h"

/*
ABattleRoyaleGameMode::ABattleRoyaleGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Player/BP_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	//PlayersNum = GetNumPlayers();
}

void ABattleRoyaleGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	//PlayersNum++;

	if (OnPlayersNumUpdated.IsBound())
	{
		OnPlayersNumUpdated.Broadcast(GetNumPlayers());
	}
}


void ABattleRoyaleGameMode::Respawn(AController* Controller)
{
	if (Controller)
	{
		if (GetLocalRole() == ROLE_Authority)
		{
			FVector Location = FVector(400.0f, 50.0f, 200.0f);
			FRotator Rotator = FRotator::ZeroRotator;
			if (APawn* Pawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, Location, Rotator))
			{
				Controller->Possess(Pawn);
			}
		}
	}
}

/*
void ABattleRoyaleGameMode::UpdatePlayers(ABattleRoyalePlayerController* PC)
{
	this->RemovePlayerControllerFromPlayerCount(PC);
}
*/