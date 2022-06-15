// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank/TankWheel.h"
#include "TireConfig.h"

UTankWheel::UTankWheel()
{
	Mass = 100.0f;
	ShapeRadius = 32.3325f;
	ShapeWidth = 49.376f;
	bAffectedByHandbrake = true;
	SteerAngle = 0.0f;
	LatStiffMaxLoad = 1.0f;
	LatStiffValue = 5.0f;
	SuspensionForceOffset = 10.0f;
	SuspensionMaxRaise = 20.0f;
	SuspensionNaturalFrequency = 20.0f;
	SuspensionDampingRatio = 0.2f;
	SuspensionDirection = FVector(0.0f, 0.0f, -1.0f);
	if (IsValid(TireConfig))
	{
		TireConfig->SetFrictionScale(1.5f);
	}
}