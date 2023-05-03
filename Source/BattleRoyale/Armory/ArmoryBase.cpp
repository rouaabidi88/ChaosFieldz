// Fill out your copyright notice in the Description page of Project Settings.


#include "ArmoryBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "BattleRoyale/Survival/LineTrace.h"


AArmoryBase::AArmoryBase()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	RootComponent = MeshComp;

	ArmoryName = FName("");
}

void AArmoryBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (ArmoryName != FName(""))
	{
		SetUpArmory(ArmoryName);
	}
}

void AArmoryBase::SetUpArmory(FName Name)
{
	if (ArmoryDataTable)
	{
		static const FString AString = FString("");
		FArmoryData* ArmoryData = ArmoryDataTable->FindRow<FArmoryData>(Name, AString, true);

		if (ArmoryData)
		{
			MeshComp->SetSkeletalMesh(ArmoryData->ArmoryMesh);
		}
	}
}

