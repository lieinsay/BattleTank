// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class ATank;
class URadialForceComponent;
class UProjectileMovementComponent;
UCLASS()
class BATTLETANK_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectile();
	UPROPERTY(BlueprintReadWrite)
		ATank* TankReflection;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override;
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
		float ProjectileDamage = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
		float ProjectileSpeed = 0;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		UProjectileMovementComponent* ProjectileMovement = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		UStaticMeshComponent* CollisionMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Components")
		UParticleSystem* LaunchBlast = nullptr;

	UPROPERTY(EditAnywhere, Category = "Components")
		UParticleSystem* ImpactBlast = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		URadialForceComponent* ExplosionForce = nullptr;
public:
	UFUNCTION(NetMulticast, Reliable)
		void LaunchProjectile();
	UFUNCTION()
		float GetProjectileSpeed() { return ProjectileSpeed; }

};
