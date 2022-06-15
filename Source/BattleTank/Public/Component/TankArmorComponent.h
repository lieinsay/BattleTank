// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TankArmorComponent.generated.h"


class ATank;
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BATTLETANK_API UTankArmorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTankArmorComponent();

	/* Get and Set */
	UFUNCTION(BlueprintPure, Category = "Armor")
		float GetDefensive() { return Defensive; }
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Armor")
		void AddDefensive(float DefensiveToSet);
	UFUNCTION(BlueprintCallable, Category = "Armor")
		float GetDefensivePercent() const { return (float)Defensive / (float)200; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UPROPERTY()
		ATank* TankReflection;
	// 当前防御力
	UPROPERTY(ReplicatedUsing = OnRep_Defensive, VisibleAnywhere, Category = "Health")
		float Defensive;
	UFUNCTION()
		void OnRep_Defensive();
	void OnDefensiveUpdate();


public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
