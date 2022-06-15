// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tank.h"
#include "GameFramework/Actor.h"
#include "TankItem.generated.h"

class USphereComponent;

UCLASS()
class BATTLETANK_API ATankItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATankItem();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure)
		FTankItemMeta GetTankItemData() const;

	UFUNCTION(BlueprintImplementableEvent)
		void Interact(APlayerController* controller);

	void SetData(FName ItemIDToSet, int32 ValueToSet);

	int32 GetOutID() { return OutID; }

	void SetOutID(int32 OutIDToSet);

	int32 GetValue() { return Value; };
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 OutID;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
		FName ItemID;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
		int32 Value;

	UPROPERTY(EditAnywhere, Category = "Components")
		USphereComponent* Collision;

	UPROPERTY(EditAnywhere, Category = "Components")
		UStaticMeshComponent* Mesh = nullptr;

};
