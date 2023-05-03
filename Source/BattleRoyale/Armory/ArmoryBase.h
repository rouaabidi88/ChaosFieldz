// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "ArmoryBase.generated.h"

UENUM(BlueprintType)
enum class EArmoryType : uint8
{
	EAmmo UMETA(DisplayName = "Ammo"),
	EMagazine UMETA(DisplayName = "Magazine"),
	EScope UMETA(DisplayName = "Scope")
};

USTRUCT(BlueprintType)
struct FArmoryData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		USkeletalMesh* ArmoryMesh;
};

UCLASS(ABSTRACT)
class BATTLEROYALE_API AArmoryBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AArmoryBase();

protected:
	virtual void BeginPlay() override;

public:
	void SetUpArmory(FName Name);

public:
	UPROPERTY(EditAnywhere)
		FName ArmoryName;

	UPROPERTY(EditAnywhere)
		class USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere)
		EArmoryType ArmoryType;

	UPROPERTY(EditAnywhere)
		class UDataTable* ArmoryDataTable;
};
