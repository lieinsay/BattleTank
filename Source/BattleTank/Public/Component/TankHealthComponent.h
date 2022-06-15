// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TankHealthComponent.generated.h"


class ATank;
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BATTLETANK_API UTankHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTankHealthComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	/*
	 * Health 相关属性的get set
	 */

	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetMaxHealth() const { return StartingHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	/*
	 * 当前生命值的百分比函数。
	 */
	UFUNCTION(BlueprintPure, Category = "Health")
		float GetHealthPercent() const { return (float)CurrentHealth / (float)StartingHealth; }

	/*
	 * 当前生命值的存值函数。
	 * 将此值的范围限定在0到MaxHealth之间，并调用OnHealthUpdate。
	 * 仅在服务器上调用。
	 */
	UFUNCTION(BlueprintCallable, Category = "Health")
		void SetCurrentHealth(float healthValue);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY()
		ATank* TankReflection;
	ATank* GetTank();
	// 最大生命值
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
		float StartingHealth = 100;
	// 当前生命值
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, VisibleAnywhere, Category = "Health")
		float CurrentHealth;

	UFUNCTION()
		void OnRep_CurrentHealth();
	void OnHealthUpdate();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
};
