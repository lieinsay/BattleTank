// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/TankPlayerController.h"

#include "BattleTankGameMode.h"
#include "Tank/Tank.h"
#include "Component/TankAimingComponent.h"
#include "Tank/PlayerTank.h"

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ATankPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	auto PossessedTank = Cast<APlayerTank>(GetPawn());
	if (PossessedTank && PossessedTank->IsUsing())AimTowardsCrossHair();
}

void ATankPlayerController::AimTowardsCrossHair() const
{
	if (!GetPawn()) { return; } // e.g. if not possessing
	if (IsLocalPlayerController())
	{
		auto Aiming = Cast<APlayerTank>(GetPawn())->GetTankAiming();
		if (!Aiming) { return; }
		FVector HitLocation;
		GetSightRayHitLocation(HitLocation);
		Aiming->AimAt(HitLocation);
	}
}

void ATankPlayerController::GetSightRayHitLocation(FVector& HitLocation) const
{
	int32 ViewportSizeX;
	int32 ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);
	auto ScreenLocation = FVector2D(ViewportSizeX * CrossHairXLocation, ViewportSizeY * CrossHairYLocation);
	FVector CameraWorldLocation;
	FVector LookDirection;
	if (!DeprojectScreenPositionToWorld(ScreenLocation.X, ScreenLocation.Y, CameraWorldLocation, LookDirection)) { return; }
	GetLookVectorHitLocation(LookDirection, HitLocation);
}

void ATankPlayerController::GetLookVectorHitLocation(FVector LookDirection, FVector& HitLocation) const
{
	FHitResult HitResult;
	auto StartLocation = PlayerCameraManager->GetCameraLocation();
	auto EndLocation = StartLocation + (LookDirection * LineTraceRange);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_Camera))
	{
		HitLocation = HitResult.Location;
	}
	else
	{
		HitLocation = EndLocation;
	}
}