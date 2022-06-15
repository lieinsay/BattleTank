// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TankAimingComponent.generated.h"

class ATank;
class AProjectile;

// Enum
UENUM(BlueprintType)
enum class EFiringState : uint8
{
	Reloading,
	Aiming,
	Locked,
	OutOfAmmo
};

UENUM()
enum class EMoveState : uint8
{
	OneGear,
	TwoGear,
	ThreeGear,
	FourGear
};

// Holds barrel's properties and Elevate method
UCLASS()
class BATTLETANK_API UTankAimingComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY()
		ATank* TankReflection;
	ATank* GetTank();

	UPROPERTY(BlueprintReadOnly, Category = State)
		EFiringState FiringState;

	UPROPERTY()
		EMoveState MoveState;

	// 打出的子弹
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
		TSubclassOf<AProjectile> ProjectileBlueprint = nullptr;
	// 开火忽略的物体
	UPROPERTY(EditAnywhere, Category = "Firing")
		TArray<AActor*> Ignore;
	// 炮台最大移动速度
	UPROPERTY(EditAnywhere, Category = Setup)
		float MaxDegreesPerSecond = 45;
	// 上弹速度
	UPROPERTY(EditDefaultsOnly, Category = "Firing")
		float ReloadTimeInSeconds = 1;
	// 最后开火时间
	double LastFireTime;
	// 子弹开火速度
	UPROPERTY(Replicated)
		float speed;
	// Tank TurretRotation
	UPROPERTY()
		float TurretRotation;
	// Tank TurretElevation
	UPROPERTY()
		float TurretElevation;
	// 出射方向
	UPROPERTY(Replicated)
		FVector AimDirection;
	// 炮台的实时预落点
	UPROPERTY(Replicated)
		FVector ForecastLocation;

public:
	// Sets default values for this component's properties
	UTankAimingComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* Tank 瞄准逻辑 */

	/* 获取瞄准状态 */
	UFUNCTION(BlueprintPure)
		EFiringState GetFiringState();

	// 玩家瞄准并通知服务器
	UFUNCTION(BlueprintCallable)
		void AimAt(FVector HitLocation);
	/*
	 * 服务器接到通知，改变自己的状态并多播
	 */
	UFUNCTION(Server, Reliable)
		void ServerAimAt(FVector AimDirectionTarget);
	/*
	 * 炮台瞄准
	 * 由Server通知所有Client调用
	 */
	UFUNCTION(NetMulticast, Reliable)
		void MoveBarrelTowards(FVector AimDirectionTarget);
	/*
	 * 移动炮台底座
	 * 由Server-MoveBarrelTowards调用
	 */
	void TurretRotate(float RelativeSpeed);
	/*
	 * 移动炮管
	 * 由Server-MoveBarrelTowards调用
	 */
	void TurretElevate(float RelativeSpeed);
	/*
	 * 获取现在炮管瞄准的方位点
	 * 由Server-MoveBarrelTowards调用
	 */
	UFUNCTION(Server, Reliable)
		void ForecastTarget();
	/*
	 * 获取现在炮管是否在移动
	 * 由Client调用
	 */
	bool IsBarrelMoving();

	/* Tank 开火逻辑 */

	/*
	 * Server 开火
	 */
	UFUNCTION(Server, Reliable, Category = "Firing")
		void Fire();

	/*
	 * Client 开火
	 */

	void PlayerFire();
	/*
	 * AI 开火
	 */
	UFUNCTION(BlueprintCallable, Category = "Firing")
		void AIFire();
	/*
	 * Client 换子弹
	 */
	UFUNCTION(BlueprintCallable, Category = "Firing")
		void PlayerLoading();
	/*
	 * 获取炮管的状态
	 * 由Client调用
	 */
	EFiringState GetFiringState() const { return FiringState; };

	/* Get and Set*/

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Firing")
		void SetProjectileBlueprint(TSubclassOf<AActor> ProjectileToSet);

	UFUNCTION(BlueprintPure, Category = "Aiming")
		float GetTurretRotation() const { return TurretRotation; }

	UFUNCTION(BlueprintPure, Category = "Aiming")
		float GetTurretElevation() const { return TurretElevation; }

	UFUNCTION(BlueprintPure, Category = "Aiming")
		FVector GetForecastLocation() const { return ForecastLocation; }

	UFUNCTION(BlueprintPure, Category = "Aiming")
		FVector GetAimDirection() const { return AimDirection; }

	UFUNCTION(Server, Reliable)
		void SetSpeed(float speedToSet);
};
