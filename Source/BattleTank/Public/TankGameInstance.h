// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "TankGameInstance.generated.h"

/**
 *
 */
UCLASS()
class BATTLETANK_API UTankGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UDataTable* GetItemDB() const { return ItemDB; }
	float GetDefensiveById(int32 Id);
	float GetProjectileSpeedById(int32 Id);
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UDataTable* ItemDB;
};
