// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "TankSkeletalMeshComponent.generated.h"

class ATank;
/**
 *
 */
UCLASS()
class BATTLETANK_API UTankSkeletalMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UTankSkeletalMeshComponent();

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
protected:
	UPROPERTY()
		ATank* TankReflection;
	ATank* GetTank();
	virtual void BeginPlay() override;
};
