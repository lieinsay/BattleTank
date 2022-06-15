// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/TankArmorComponent.h"

#include "Net/UnrealNetwork.h"
#include "Tank/Tank.h"

// Sets default values for this component's properties
UTankArmorComponent::UTankArmorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	// ...
	Defensive = 0;
}

void UTankArmorComponent::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 复制当前护甲值。
	DOREPLIFETIME(UTankArmorComponent, Defensive);
}

void UTankArmorComponent::AddDefensive_Implementation(float DefensiveToSet)
{
	Defensive += DefensiveToSet;
	if (Defensive > 200) Defensive = 200;
	OnDefensiveUpdate();
}

// Called when the game starts
void UTankArmorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	TankReflection = Cast<ATank>(GetOwner());
}


// Called every frame
void UTankArmorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTankArmorComponent::OnRep_Defensive()
{
	OnDefensiveUpdate();
}

void UTankArmorComponent::OnDefensiveUpdate()
{

}
