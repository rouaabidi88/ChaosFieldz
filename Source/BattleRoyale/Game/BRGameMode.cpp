// Fill out your copyright notice in the Description page of Project Settings.


#include "BRGameMode.h"
#include "GameFramework/PlayerController.h"

#include "BattleRoyale/BattleRoyaleCharacter.h"
#include "BattleRoyale/UMG/MainWidget.h"


ABRGameMode::ABRGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Player/BP_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ABRGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (OnPlayersNumUpdated.IsBound())
	{
		OnPlayersNumUpdated.Broadcast(GetNumPlayers());
	}
}

void ABRGameMode::UpdatePlayers(APlayerController* PC)
{
	RemovePlayerControllerFromPlayerCount(PC);
	if (OnPlayersNumUpdated.IsBound())
	{
		OnPlayersNumUpdated.Broadcast(GetNumPlayers());
	}
}

void ABRGameMode::Respawn(AController* Controller)
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