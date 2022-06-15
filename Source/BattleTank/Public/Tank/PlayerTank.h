// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tank/Tank.h"
#include "PlayerTank.generated.h"

class UCameraComponent;
class USpringArmComponent;
/**
 *
 */
UCLASS()
class BATTLETANK_API APlayerTank : public ATank
{
	GENERATED_BODY()
public:
	APlayerTank();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* 移动 */

	// 前进后退
	void MoveForward(float Value);
	void MoveRight(float Value);

	/* 开火 */
	void Fire();

	/* 移动视角 */
	void Azimuth(float DeltaTime);
	void Elevation(float DeltaTime);

	/* 是否可以操作 */
	bool IsUsing();

	/* 获得排行榜 */
	UFUNCTION(BlueprintCallable)
		void  GetTankListFString(TMap<FString, int32> TankMap);
	UFUNCTION(BlueprintCallable)
		void  GetTankListint32(TMap<FString, int32> TankMap);
	UFUNCTION(BlueprintCallable)
		void  GetTankTeamData(int32 A, int32 B);

protected:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USceneComponent* AzimuthGimbal;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USpringArmComponent* SpringArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UCameraComponent* CameraComponent;

	int32 MouseSensitivity;
	UPROPERTY(ReplicatedUsing = OnRep_TankListString, BlueprintReadWrite)
		TArray<FString> TankListString;
	UPROPERTY(ReplicatedUsing = OnRep_TankListInt, BlueprintReadWrite)
		TArray<int32> TankListInt;
	UPROPERTY(ReplicatedUsing = OnRep_TeamAData, BlueprintReadWrite)
		int32 TeamAData;
	UPROPERTY(ReplicatedUsing = OnRep_TeamBData, BlueprintReadWrite)
		int32 TeamBData;
	UFUNCTION(BlueprintImplementableEvent)
		void OnRep_TankListString();
	UFUNCTION(BlueprintImplementableEvent)
		void OnRep_TankListInt();
	UFUNCTION(BlueprintImplementableEvent)
		void OnRep_TeamAData();
	UFUNCTION(BlueprintImplementableEvent)
		void OnRep_TeamBData();
};
