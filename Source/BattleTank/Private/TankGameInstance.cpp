// Fill out your copyright notice in the Description page of Project Settings.


#include "TankGameInstance.h"


float UTankGameInstance::GetDefensiveById(int32 Id)
{
	return ((Id - 8) * 10);
}

float UTankGameInstance::GetProjectileSpeedById(int32 Id)
{
	switch (Id)
	{
	case 1:return 10000;
	case 2:return 3000;
	case 3:return 3000;
	case 4:return 14000;
	case 5:return 50000;
	case 6:return 8000;
	case 7:return 20000;
	case 8:return 10000;
	default:return 0;
	}
}
