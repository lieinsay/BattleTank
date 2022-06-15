// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Tank/Tank.h"
#include "TankEquipmentComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BATTLETANK_API UTankEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTankEquipmentComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY()
		ATank* TankReflection;
	ATank* GetTank();

	FTimerHandle ArmorTimer;

	/*
	 * 玩家当前装备的炮弹
	 */
	UPROPERTY()
		FTankItemMeta NowProjectile;
	/*
	 * 玩家当前装备的护甲
	 */
	UPROPERTY()
		TArray<FTankItemMeta> NowArmor;
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category = "Equipment")
		int32 GetAmmoCount();

	UFUNCTION(BlueprintPure, Category = "Equipment")
		UTexture2D* GetProjectileImage();
	UFUNCTION(BlueprintPure, Category = "Equipment")
		FTankItemMeta GetNowProjectile() { return NowProjectile; }

	UFUNCTION(BlueprintPure, Category = "Equipment")
		TArray<FTankItemMeta> GetNowArmor() { return NowArmor; }

	/*
	 * 装备炮弹
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
		void InstallNowProjectile(FTankItemMeta Item);
	/*
	 * 移除当前炮弹
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
		void UninstallNowProjectile(FTankItemMeta Item);

	/*
	 * 使用当前子弹
	 */
	UFUNCTION(Category = "Equipment")
		void UseNowProjectile();

	/*
	 * 增加装备的护甲，以队列的方式插入护甲，如果已经装备则脱掉
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
		void ChangeNowArmor(FTankItemMeta Item);

	/*
	 * 护甲随着时间减少
	 */
	UFUNCTION(Category = "Equipment")
		void UsingNowArmor();
};
