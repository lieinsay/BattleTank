// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/TankSkeletalMeshComponent.h"

#include "Component/TankInventoryComponent.h"
#include "Tank/TankItem.h"

UTankSkeletalMeshComponent::UTankSkeletalMeshComponent()
{
	SetIsReplicatedByDefault(true);
	OnComponentBeginOverlap.AddDynamic(this, &UTankSkeletalMeshComponent::OnOverlapBegin);
	OnComponentEndOverlap.AddDynamic(this, &UTankSkeletalMeshComponent::OnOverlapEnd);
}

void UTankSkeletalMeshComponent::BeginPlay()
{
	Super::BeginPlay();
	TankReflection = Cast<ATank>(GetOwner());
}

ATank* UTankSkeletalMeshComponent::GetTank()
{
	if (IsValid(TankReflection))
	{
		return TankReflection;
	}
	TankReflection = Cast<ATank>(GetOwner());
	return Cast<ATank>(GetOwner());
}

void UTankSkeletalMeshComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetTank()->GetLocalRole() == ROLE_Authority)
	{
		auto TankItemRef = Cast<ATankItem>(OtherActor);
		if (TankItemRef)
		{
			GetTank()->GetTankInventory()->AddItemToNearby(TankItemRef->GetTankItemData(), TankItemRef);
		}
	}
}

void UTankSkeletalMeshComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GetTank()->GetLocalRole() == ROLE_Authority)
	{
		auto TankItemRef = Cast<ATankItem>(OtherActor);
		if (TankItemRef)
		{
			GetTank()->GetTankInventory()->RemoveItemToNearby(TankItemRef->GetTankItemData());
		}
	}
}
