// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/TankAimingComponent.h"

#include "TankGameInstance.h"
#include "Component/TankSkeletalMeshComponent.h"
#include "Ammo/Projectile.h"
#include "Component/TankEquipmentComponent.h"
#include "Component/TankInventoryComponent.h"
#include "Tank/Tank.h"
#include "Engine/StaticMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UTankAimingComponent::UTankAimingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	// ...
	TurretRotation = 0.f;
	TurretElevation = 0.f;
	FiringState = EFiringState::Reloading;
	MoveState = EMoveState::OneGear;
	LastFireTime = -1;
}

// Called when the game starts
void UTankAimingComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
	TankReflection = Cast<ATank>(GetOwner());
}

void UTankAimingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTankAimingComponent, ForecastLocation);
	DOREPLIFETIME(UTankAimingComponent, AimDirection);
	DOREPLIFETIME(UTankAimingComponent, speed);
}

// Called every frame
void UTankAimingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

EFiringState UTankAimingComponent::GetFiringState()
{
	auto Equipment = GetTank()->GetTankEquipment();
	if (!Equipment)
	{
		FiringState = EFiringState::OutOfAmmo;
		return FiringState;
	}
	auto AmmoCount = Equipment->GetAmmoCount();
	if (AmmoCount <= 0)
	{
		FiringState = EFiringState::OutOfAmmo;
	}
	else if ((FPlatformTime::Seconds() - LastFireTime) < ReloadTimeInSeconds)
	{
		FiringState = EFiringState::Reloading;
	}
	else if (IsBarrelMoving())
	{
		FiringState = EFiringState::Aiming;
	}
	else
	{
		FiringState = EFiringState::Locked;
	}
	return FiringState;
}

ATank* UTankAimingComponent::GetTank()
{
	if (IsValid(TankReflection))
	{
		return TankReflection;
	}
	TankReflection = Cast<ATank>(GetOwner());
	return Cast<ATank>(GetOwner());
}

void UTankAimingComponent::AimAt(FVector HitLocation)
{
	auto Mesh = GetTank()->GetTankSkeletalMesh();
	if (!Mesh) { return; }
	// 设置标记点
	FVector OutLaunchVelocity;
	FVector StartLocation = Mesh->GetSocketLocation(FName("gunSocket"));
	bool bHaveAimSolution = UGameplayStatics::SuggestProjectileVelocity(
		this,
		OutLaunchVelocity,
		StartLocation,
		HitLocation,
		speed,
		false,
		1,
		0,
		ESuggestProjVelocityTraceOption::DoNotTrace
	);
	// 计算出射的方向(单位向量)
	if (bHaveAimSolution)
		AimDirection = OutLaunchVelocity.GetSafeNormal();
	else
		AimDirection = (HitLocation - StartLocation).Rotation().Vector();
	ServerAimAt(AimDirection);
}

void UTankAimingComponent::ServerAimAt_Implementation(FVector AimDirectionTarget)
{
	MoveBarrelTowards(AimDirectionTarget);
	ForecastTarget();
}

void UTankAimingComponent::MoveBarrelTowards_Implementation(FVector AimDirectionTarget)
{
	auto SkeletalMesh = GetTank()->GetTankSkeletalMesh();
	if (!SkeletalMesh) { return; }
	auto BarrelDirection = SkeletalMesh->GetSocketRotation(FName("gunSocket")).Vector();
	auto PlayerDirection = AimDirectionTarget;
	auto CrossDirection = FVector::CrossProduct(BarrelDirection, PlayerDirection);
	auto DeltaRotator = PlayerDirection.Rotation() - BarrelDirection.Rotation();
	if (FMath::Abs(CrossDirection.Z) > 0.005)
	{
		// // 改变转动速度
		if (FMath::Abs(DeltaRotator.Yaw) >= 60)
		{
			MoveState = EMoveState::FourGear;
		}
		else if (FMath::Abs(DeltaRotator.Yaw) >= 30)
		{
			MoveState = EMoveState::ThreeGear;
		}
		else if (FMath::Abs(DeltaRotator.Yaw) >= 10)
		{
			MoveState = EMoveState::TwoGear;
		}
		else
		{
			MoveState = EMoveState::OneGear;
		}
		if (FMath::Abs(DeltaRotator.Yaw) < 180)
		{
			TurretRotate(DeltaRotator.Yaw);
		}
		else // Avoid going the long-way round
		{
			TurretRotate(-DeltaRotator.Yaw);
		}

	}
	// // Always yaw the shortest way
	auto AimAsRotator = PlayerDirection.Rotation();
	auto BarrelRotator = BarrelDirection.Rotation();
	if (FMath::Abs(DeltaRotator.Pitch) > 0.1 && AimAsRotator.Pitch >= -5 && AimAsRotator.Pitch <= 20)
	{
		TurretElevate(DeltaRotator.Pitch);
	}
	else if (AimAsRotator.Pitch < -5 && BarrelRotator.Pitch > -5)
	{
		TurretElevate(-1);
	}
	else if (AimAsRotator.Pitch > 20 && BarrelRotator.Pitch < 20)
	{
		TurretElevate(1);
	}

}

void UTankAimingComponent::TurretRotate(float RelativeSpeed)
{
	// 在此帧内将炮筒左右移动正确的量
	// 给定最大提升速度和帧时间
	RelativeSpeed = FMath::Clamp<float>(RelativeSpeed, -1, +1);
	auto RotationChange = FMath::Floor(RelativeSpeed * ((float)MoveState + 1) * MaxDegreesPerSecond) / 100;
	TurretRotation = TurretRotation + RotationChange;
}

void UTankAimingComponent::TurretElevate(float RelativeSpeed)
{
	// 在此帧内将炮筒上下移动正确的量
	// 给定最大提升速度和帧时间
	RelativeSpeed = FMath::Clamp<float>(RelativeSpeed, -1, +1);
	auto RotationChange = (FMath::Floor(RelativeSpeed * MaxDegreesPerSecond)) / 100;
	TurretElevation = TurretElevation + RotationChange;
}

void UTankAimingComponent::ForecastTarget_Implementation()
{
	auto Mesh = GetTank()->GetTankSkeletalMesh();
	if (!Mesh) { return; }
	auto gunR = GetTank()->GetTankSkeletalMesh()->GetSocketRotation(FName("gunSocket")).Vector();
	auto StartLocation = GetTank()->GetTankSkeletalMesh()->GetSocketLocation(FName("gunSocket"));
	FHitResult HitResult;
	TArray<FVector> OutPathPositions;
	UGameplayStatics::Blueprint_PredictProjectilePath_ByTraceChannel(
		this,
		HitResult,
		OutPathPositions,
		ForecastLocation,
		StartLocation,
		gunR * speed,
		true,
		25,
		ECollisionChannel::ECC_WorldDynamic,
		false,
		Ignore,
		EDrawDebugTrace::None,
		0,
		15,
		5,
		0
	);
}

bool UTankAimingComponent::IsBarrelMoving()
{
	return !(GetTank()->GetTankSkeletalMesh()->GetSocketRotation(FName("gunSocket")).Vector().Equals(AimDirection, 0.05)); // vectors are equal
}

void UTankAimingComponent::Fire_Implementation()
{
	auto tank = GetTank()->GetTankSkeletalMesh();
	if (!tank || !ProjectileBlueprint) { return; }
	auto Projectile = GetWorld()->SpawnActor<AProjectile>(
		ProjectileBlueprint,
		tank->GetSocketLocation(FName("gunSocket")),
		tank->GetSocketRotation(FName("gunSocket"))
		);
	Ignore.Add(Projectile);
	Projectile->LaunchProjectile();
	Projectile->TankReflection = GetTank();
	auto Equipment = GetTank()->GetTankEquipment();
	if (Equipment && !GetTank()->IsLocallyControlled())
	{
		Equipment->UseNowProjectile();
	}
}

void UTankAimingComponent::PlayerFire()
{
	if (FiringState == EFiringState::Locked || FiringState == EFiringState::Aiming)
	{
		PlayerLoading();
		Fire();
		GetTank()->GetTankEquipment()->UseNowProjectile();
	}
}

void UTankAimingComponent::AIFire()
{
	Fire();
}

void UTankAimingComponent::PlayerLoading()
{
	LastFireTime = FPlatformTime::Seconds();
}

void UTankAimingComponent::SetSpeed_Implementation(float speedToSet)
{
	speed = speedToSet;
}

void UTankAimingComponent::SetProjectileBlueprint_Implementation(TSubclassOf<AActor> ProjectileToSet)
{
	ProjectileBlueprint = ProjectileToSet;
}
