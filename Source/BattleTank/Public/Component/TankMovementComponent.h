// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleWheeledVehicleMovementComponent.h"
#include "TankMovementComponent.generated.h"


class ATank;

UENUM()
enum class EMyMoveState : uint8
{
	Forward,
	Back,
	Stop
};

UENUM()
enum class ETurnState : uint8
{
	Left,
	Right,
	Stop
};

class UTankTrack;
/**
 *
 */
UCLASS()
class BATTLETANK_API UTankMovementComponent : public USimpleWheeledVehicleMovementComponent
{
	GENERATED_BODY()

public:
	UTankMovementComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Movement")
		EMyMoveState GetMoveState() { return MoveState; };
	UFUNCTION(BlueprintPure, Category = "Movement")
		ETurnState GetTurnState() { return TurnState; };
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Input")
		void IntendMoveForward(float Throw);
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Input")
		void IntendTurnRight(float Throw);

	UFUNCTION(BlueprintPure, Category = "Input")
		float GetSpeed() { return Speed; }
	void SetSpeed(float Percent);
protected:
	/*Tank的移动参数*/
	UPROPERTY(ReplicatedUsing = OnRep_MoveState)
		EMyMoveState MoveState;

	UPROPERTY(ReplicatedUsing = OnRep_TurnState)
		ETurnState TurnState;

	UFUNCTION()
		void OnRep_MoveState();
	UFUNCTION()
		void OnRep_TurnState();

	UPROPERTY()
		ATank* tankReflection;

	void Move();

	UPROPERTY(EditAnywhere, Category = "Setup")
		float MaxSpeed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Setup")
		float Speed;

public:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
