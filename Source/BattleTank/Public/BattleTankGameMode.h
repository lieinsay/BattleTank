// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "Tank/Tank.h"
#include "BattleTankGameMode.generated.h"

class AItemStart;
/**
 *
 */
UCLASS()
class BATTLETANK_API ABattleTankGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void StartPlay() override;
	void AddTankItem() { TankItemCount++; }
	void RemoveTankItem() { TankItemCount--; }
	void SpawnTankItem();

	/* 击中得分，被打扣分 */
	void HitGrade(ATank* A, ATank* B);
	/* 击杀得分，被被杀不扣分 */
	void KillGrade(ATank* A, ATank* B);


	void SortTankList();
protected:
	TMap<FString, int32> TankMap;
	TArray<ATank*> OutTanks;
	TArray<AActor*> OutItemStarts;
	int32 TankItemCount;
	int32 ItemStartPointCount;
	FTimerHandle SpawnItemTimer;
	int32 TeamAData;
	int32 TeamBData;
};