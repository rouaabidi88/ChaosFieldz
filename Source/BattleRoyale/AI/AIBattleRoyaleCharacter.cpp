// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBattleRoyaleCharacter.h"

// Sets default values
AAIBattleRoyaleCharacter::AAIBattleRoyaleCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAIBattleRoyaleCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAIBattleRoyaleCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAIBattleRoyaleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

