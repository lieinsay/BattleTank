// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Tank/Tank.h"
#include "TankPlayerController.generated.h"

class UTankAimingComponent;
class ATank;

/**
 *
 */
UCLASS()
class BATTLETANK_API ATankPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/* 玩家 瞄准逻辑 */

	/*
	 * 向准星瞄准
	 */
	void AimTowardsCrossHair() const;
	/*
	 * 获取瞄准线命中位置
	 */
	void GetSightRayHitLocation(FVector& HitLocation) const;
	/*
	 * 获取查看向量命中位置
	 * LineTraceRange
	 */
	void GetLookVectorHitLocation(FVector LookDirection, FVector& HitLocation) const;
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/*
	 * 玩家 瞄准相关参数
	 */

	 // 十字线X定位
	UPROPERTY(EditDefaultsOnly)
		float CrossHairXLocation = 0.5;
	// 十字线Y定位
	UPROPERTY(EditDefaultsOnly)
		float CrossHairYLocation = 0.3333;
	// 向量命中位置的最远距离
	UPROPERTY(EditDefaultsOnly)
		float LineTraceRange = 50000;
};
