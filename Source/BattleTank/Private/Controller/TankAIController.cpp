// Fill out your copyright notice in the Description page of Project Settings.

#include "Controller/TankAIController.h"
#include "Tank/Tank.h"
#include "Component/TankAimingComponent.h"

void ATankAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ATankAIController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (InPawn)
	{
		auto PossessedTank = Cast<ATank>(InPawn);
		if (!ensure(PossessedTank)) { return; }

		// Subscribe our local method to the tank's death event
		PossessedTank->OnDeath.AddUniqueDynamic(this, &ATankAIController::OnPossedTankDeath);
	}
}

void ATankAIController::OnPossedTankDeath()
{
	if (!ensure(GetPawn())) { return; } // TODO remove if ok
	GetPawn()->DetachFromControllerPendingDestroy();
}

void ATankAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	auto PlayerTank = Cast<ATank>(GetWorld()->GetFirstPlayerController()->GetPawn());
	auto ControlledTank = Cast<ATank>(GetPawn());
	auto Aiming = ControlledTank->GetTankAiming();
	if (!PlayerTank || !ControlledTank || !Aiming) { return; }
	Aiming->AimAt(PlayerTank->GetActorLocation());
}