// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VehicleWheel.h"
#include "TankWheel.generated.h"

/**
 * 
 */
UCLASS()
class BATTLETANK_API UTankWheel : public UVehicleWheel
{
	GENERATED_BODY()

public:
	UTankWheel();
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Suspension")
	FVector SuspensionDirection;
};
