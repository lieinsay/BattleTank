// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank/PlayerTank.h"

#include "Camera/CameraComponent.h"
#include "Component/TankAimingComponent.h"
#include "Component/TankHealthComponent.h"
#include "Component/TankMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

APlayerTank::APlayerTank()
{
	AzimuthGimbal = CreateDefaultSubobject<USceneComponent>(TEXT("AzimuthGimbal"));
	AzimuthGimbal->SetupAttachment(GetRootComponent());

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringThird"));
	SpringArmComponent->SetupAttachment(AzimuthGimbal);
	SpringArmComponent->SetRelativeRotation(FRotator(-20.f, 0, 0));
	SpringArmComponent->SetRelativeLocation(FVector(0, 0, 150.f));
	SpringArmComponent->TargetArmLength = 1500.f;
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("3rd"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	MouseSensitivity = 100;
}

void APlayerTank::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerTank, TankListString);
	DOREPLIFETIME(APlayerTank, TankListInt);
	DOREPLIFETIME(APlayerTank, TeamAData);
	DOREPLIFETIME(APlayerTank, TeamBData);
}

void APlayerTank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerTank::Fire);
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerTank::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerTank::MoveRight);
	PlayerInputComponent->BindAxis("AimAzimuth", this, &APlayerTank::Azimuth);
	PlayerInputComponent->BindAxis("AimElevation", this, &APlayerTank::Elevation);
}

void APlayerTank::MoveForward(float Value)
{
	if (IsUsing())GetTankMovement()->IntendMoveForward(Value);
}

void APlayerTank::MoveRight(float Value)
{
	if (IsUsing())GetTankMovement()->IntendTurnRight(Value);
}


void APlayerTank::Fire()
{
	if (IsUsing())GetTankAiming()->PlayerFire();
}

void APlayerTank::Azimuth(float DeltaTime)
{
	AzimuthGimbal->AddLocalRotation(FRotator(0, DeltaTime * MouseSensitivity * GetWorld()->GetDeltaSeconds(), 0));
}

void APlayerTank::Elevation(float DeltaTime)
{
	SpringArmComponent->AddLocalRotation(FRotator(DeltaTime * MouseSensitivity * GetWorld()->GetDeltaSeconds(), 0, 0));
}

bool APlayerTank::IsUsing()
{
	auto life = TankHealth->GetCurrentHealth();
	if (life > 0)return true;
	return false;
}

void APlayerTank::GetTankListFString(TMap<FString, int32> TankMap)
{
	TankListString = {};
	for (auto Map : TankMap)
	{
		TankListString.Add(Map.Key);
	}
}

void APlayerTank::GetTankListint32(TMap<FString, int32> TankMap)
{
	TankListInt = {};
	for (auto Map : TankMap)
	{
		TankListInt.Add(Map.Value);
	}
}

void APlayerTank::GetTankTeamData(int32 A, int32 B)
{
	TeamAData = A;
	TeamBData = B;
}

