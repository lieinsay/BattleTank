// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/TankHealthComponent.h"

#include "Component/TankMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Tank/Tank.h"

// Sets default values for this component's properties
UTankHealthComponent::UTankHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	// ...
}

void UTankHealthComponent::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 复制当前生命值。
	DOREPLIFETIME(UTankHealthComponent, CurrentHealth);

}

// Called when the game starts
void UTankHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CurrentHealth = StartingHealth;
	TankReflection = Cast<ATank>(GetOwner());
}


// Called every frame
void UTankHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTankHealthComponent::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

ATank* UTankHealthComponent::GetTank()
{
	if (IsValid(TankReflection))
	{
		return TankReflection;
	}
	TankReflection = Cast<ATank>(GetOwner());
	return Cast<ATank>(GetOwner());
}

void UTankHealthComponent::OnHealthUpdate()
{
	if (!TankReflection) { return; }
	//客户端特定的功能
	if (TankReflection->IsLocallyControlled())
	{
		GetTank()->GetTankMovement()->SetSpeed(GetHealthPercent());
		// FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), GetTank()->GetTankMovement()->GetSpeed());
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}

	//服务器特定的功能
	if (TankReflection->GetLocalRole() == ROLE_Authority)
	{
		GetTank()->GetTankMovement()->SetSpeed(GetHealthPercent());
		// FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), GetTank()->GetTankMovement()->GetSpeed());
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}

	//在所有机器上都执行的函数。 
	/*
		因任何因伤害或死亡而产生的特殊功能都应放在这里。
	*/
}

void UTankHealthComponent::SetCurrentHealth(float healthValue)
{
	if (!TankReflection) { return; }
	if (TankReflection->GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp<float>(healthValue, 0.f, StartingHealth);
		OnHealthUpdate();
	}
}