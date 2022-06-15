// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/TankMovementComponent.h"
#include "PhysXVehicleManager.h"
#include "Net/UnrealNetwork.h"
#include "Tank/Tank.h"

UTankMovementComponent::UTankMovementComponent()
{
	MoveState = EMyMoveState::Stop;
	TurnState = ETurnState::Stop;
	MaxSpeed = 5000;
}

void UTankMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// 复制当前移动方式。
	DOREPLIFETIME(UTankMovementComponent, MoveState);
	DOREPLIFETIME(UTankMovementComponent, TurnState);
}

void UTankMovementComponent::SetSpeed(float Percent)
{
	Speed = MaxSpeed * Percent;
}

void UTankMovementComponent::OnRep_MoveState()
{

}

void UTankMovementComponent::OnRep_TurnState()
{

}

void UTankMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	tankReflection = Cast<ATank>(GetOwner());
	Speed = MaxSpeed;
}

void UTankMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Move();
}

void UTankMovementComponent::IntendMoveForward_Implementation(float Throw)
{
	if (Throw == 0)
	{
		MoveState = EMyMoveState::Stop;
	}
	else if (Throw > 0)
	{
		MoveState = EMyMoveState::Forward;
	}
	else if (Throw < 0)
	{
		MoveState = EMyMoveState::Back;
	}
}

void UTankMovementComponent::IntendTurnRight_Implementation(float Throw)
{
	if (Throw == 0)
	{
		TurnState = ETurnState::Stop;
	}
	else if (Throw > 0)
	{
		TurnState = ETurnState::Right;
	}
	else if (Throw < 0)
	{
		TurnState = ETurnState::Left;
	}
}


void UTankMovementComponent::Move()
{
	if (!tankReflection) { return; }
	int32 size = Wheels.Num() / 4;
	if (MoveState == EMyMoveState::Stop && TurnState == ETurnState::Stop)
	{
		for (int i = 0; i < size * 4; i++)
		{
			SetBrakeTorque(5000000, i);
			SetSteerAngle(0, i);
			SetDriveTorque(0, i);
		}
	}
	else if (MoveState == EMyMoveState::Forward && TurnState == ETurnState::Stop)
	{
		if (GetForwardSpeed() < 0)
		{
			for (int i = 0; i < size * 4; i++)
			{
				SetBrakeTorque(5000000, i);
				SetSteerAngle(0, i);
				SetDriveTorque(0, i);
			}
		}
		else
		{
			for (int i = 0; i < Wheels.Num(); i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(0, i);
				SetDriveTorque(Speed - FMath::Clamp<float>(GetForwardSpeed() * 2, -Speed, Speed), i);
			}
		}

	}
	else if (MoveState == EMyMoveState::Back && TurnState == ETurnState::Stop)
	{
		if (GetForwardSpeed() > 0)
		{
			for (int i = 0; i < size * 4; i++)
			{
				SetBrakeTorque(5000000, i);
				SetSteerAngle(0, i);
				SetDriveTorque(0, i);
			}
		}
		else
		{
			for (int i = 0; i < size * 4; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(0, i);
				SetDriveTorque(-Speed - FMath::Clamp<float>(GetForwardSpeed() * 4, -Speed, Speed), i);
			}
		}
	}
	else if (MoveState == EMyMoveState::Stop && TurnState == ETurnState::Right)
	{
		if (FMath::Abs(GetForwardSpeed()) > 500)
		{
			for (int i = 0; i < Wheels.Num(); i++)
			{
				SetBrakeTorque(5000000, i);
				SetSteerAngle(90, i);
				SetDriveTorque(0, i);
			}
		}
		else
		{
			for (int i = 0; i < size * 1; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(33.5, i);
				SetDriveTorque(Speed, i);
			}
			for (int i = size * 1; i < size * 2; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(0, i);
				SetDriveTorque(Speed, i);
			}
			for (int i = size * 2; i < size * 3; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(40, i);
				SetDriveTorque(Speed, i);
			}
			for (int i = size * 3; i < size * 4; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(0, i);
				SetDriveTorque(Speed, i);
			}
		}
	}
	else if (MoveState == EMyMoveState::Stop && TurnState == ETurnState::Left)
	{
		if (FMath::Abs(GetForwardSpeed()) > 500)
		{
			for (int i = 0; i < Wheels.Num(); i++)
			{
				SetBrakeTorque(5000000, i);
				SetSteerAngle(-90, i);
				SetDriveTorque(0, i);
			}
		}
		else
		{
			for (int i = 0; i < size * 1; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(-40, i);
				SetDriveTorque(Speed, i);
			}
			for (int i = size * 1; i < size * 2; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(0, i);
				SetDriveTorque(Speed, i);
			}
			for (int i = size * 2; i < size * 3; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(-33.5, i);
				SetDriveTorque(Speed, i);
			}
			for (int i = size * 3; i < size * 4; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(0, i);
				SetDriveTorque(Speed, i);
			}
		}
	}
	else if (MoveState == EMyMoveState::Forward && TurnState == ETurnState::Right)
	{
		if (FMath::Abs(GetForwardSpeed()) > 1500)
		{
			for (int i = 0; i < Wheels.Num(); i++)
			{
				SetBrakeTorque(5000000, i);
				SetSteerAngle(90, i);
				SetDriveTorque(0, i);
			}
		}
		else
		{
			for (int i = 0; i < size * 1; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(16, i);
				SetDriveTorque(Speed - FMath::Clamp<float>(GetForwardSpeed() * 2, -Speed, Speed), i);
			}
			for (int i = size * 1; i < size * 2; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(0, i);
				SetDriveTorque(Speed - FMath::Clamp<float>(GetForwardSpeed() * 2, -Speed, Speed), i);
			}
			for (int i = size * 2; i < size * 3; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(20, i);
				SetDriveTorque(Speed - FMath::Clamp<float>(GetForwardSpeed() * 2, -Speed, Speed), i);
			}
			for (int i = size * 3; i < size * 4; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(0, i);
				SetDriveTorque(Speed - FMath::Clamp<float>(GetForwardSpeed() * 2, -Speed, Speed), i);
			}
		}
	}
	else if (MoveState == EMyMoveState::Forward && TurnState == ETurnState::Left)
	{
		if (FMath::Abs(GetForwardSpeed()) > 1500)
		{
			for (int i = 0; i < Wheels.Num(); i++)
			{
				SetBrakeTorque(5000000, i);
				SetSteerAngle(-90, i);
				SetDriveTorque(0, i);
			}
		}
		else
		{
			for (int i = 0; i < size * 1; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(-20, i);
				SetDriveTorque(Speed - FMath::Clamp<float>(GetForwardSpeed() * 2, -Speed, Speed), i);
			}
			for (int i = size * 1; i < size * 2; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(0, i);
				SetDriveTorque(Speed - FMath::Clamp<float>(GetForwardSpeed() * 2, -Speed, Speed), i);
			}
			for (int i = size * 2; i < size * 3; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(-16, i);
				SetDriveTorque(Speed - FMath::Clamp<float>(GetForwardSpeed() * 2, -Speed, Speed), i);
			}
			for (int i = size * 3; i < size * 4; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(0, i);
				SetDriveTorque(Speed - FMath::Clamp<float>(GetForwardSpeed() * 2, -Speed, Speed), i);
			}
		}
	}
	else if (MoveState == EMyMoveState::Back && TurnState == ETurnState::Right)
	{
		if (FMath::Abs(GetForwardSpeed()) > 1000)
		{
			for (int i = 0; i < Wheels.Num(); i++)
			{
				SetBrakeTorque(5000000, i);
				SetSteerAngle(-90, i);
				SetDriveTorque(0, i);
			}
		}
		else
		{
			for (int i = 0; i < size * 1; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(0, i);
				SetDriveTorque(-Speed - FMath::Clamp<float>(GetForwardSpeed() * 2, -Speed, Speed), i);
			}
			for (int i = size * 1; i < size * 2; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(-20, i);
				SetDriveTorque(-Speed - FMath::Clamp<float>(GetForwardSpeed() * 2, -Speed, Speed), i);
			}
			for (int i = size * 2; i < size * 3; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(0, i);
				SetDriveTorque(-Speed - FMath::Clamp<float>(GetForwardSpeed() * 2, -Speed, Speed), i);
			}
			for (int i = size * 3; i < size * 4; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(-16, i);
				SetDriveTorque(-Speed - FMath::Clamp<float>(GetForwardSpeed() * 2, -Speed, Speed), i);
			}
		}
	}
	else if (MoveState == EMyMoveState::Back && TurnState == ETurnState::Left)
	{
		if (FMath::Abs(GetForwardSpeed()) > 1000)
		{
			for (int i = 0; i < Wheels.Num(); i++)
			{
				SetBrakeTorque(5000000, i);
				SetSteerAngle(90, i);
				SetDriveTorque(0, i);
			}
		}
		else
		{
			for (int i = 0; i < size * 1; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(0, i);
				SetDriveTorque(-Speed - FMath::Clamp<float>(GetForwardSpeed() * 2, -Speed, Speed), i);
			}
			for (int i = size * 1; i < size * 2; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(16, i);
				SetDriveTorque(-Speed - FMath::Clamp<float>(GetForwardSpeed() * 2, -Speed, Speed), i);
			}
			for (int i = size * 2; i < size * 3; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(0, i);
				SetDriveTorque(-Speed - FMath::Clamp<float>(GetForwardSpeed() * 2, -Speed, Speed), i);
			}
			for (int i = size * 3; i < size * 4; i++)
			{
				SetBrakeTorque(0, i);
				SetSteerAngle(20, i);
				SetDriveTorque(-Speed - FMath::Clamp<float>(GetForwardSpeed() * 2, -Speed, Speed), i);
			}
		}
	}
}
