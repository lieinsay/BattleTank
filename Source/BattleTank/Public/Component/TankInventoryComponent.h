// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Tank/Tank.h"
#include "TankInventoryComponent.generated.h"


class AProjectile;
UCLASS()
class BATTLETANK_API UTankInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTankInventoryComponent();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY()
		ATank* TankReflection;
	ATank* GetTank();

	/*
	 * 玩家 背包相关参数
	 */

	 // 储存背包中的内容
	UPROPERTY(ReplicatedUsing = OnRep_Inventory)
		TArray<FTankItemMeta> Inventory;
	UFUNCTION()
		void OnRep_Inventory();
	// 附近中的内容
	UPROPERTY(ReplicatedUsing = OnRep_Nearby)
		TArray<FTankItemMeta> Nearby;
	UFUNCTION()
		void OnRep_Nearby();
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	 * 背包的相关逻辑
	 */

	 // 通过数据ID查找并添加到我们的数据数组中
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "InventoryUtils")
		void AddItemToInventory(FTankItemMeta Item);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "InventoryUtils")
		void RemoveItemToInventory(FTankItemMeta Item);

	UFUNCTION(BlueprintCallable, Category = "InventoryUtils")
		void AddItemToNearby(FTankItemMeta Item, ATankItem* TankItemRef);

	UFUNCTION(BlueprintCallable, Category = "InventoryUtils")
		void RemoveItemToNearby(FTankItemMeta Item);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "InventoryUtils")
		void DestroyItemToWorld(FTankItemMeta Item);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "InventoryUtils")
		void SpawnItemToWorld(FTankItemMeta Item);

	UFUNCTION(BlueprintCallable, Category = "InventoryUtils")
		TArray<FTankItemMeta> GetInventory() { return Inventory; }

	UFUNCTION(BlueprintCallable, Category = "InventoryUtils")
		TArray<FTankItemMeta> GetNearby() { return Nearby; }

	UFUNCTION()
		void UseNowProjectileToInventory(FTankItemMeta Item);

	UFUNCTION()
		void UseNowArmorToInventory(FTankItemMeta Item);

	UFUNCTION()
		void BeUsingToInventory(FTankItemMeta Item);
	UFUNCTION()
		void NoUsingToInventory(FTankItemMeta Item);
};
