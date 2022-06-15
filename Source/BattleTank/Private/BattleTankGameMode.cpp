// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleTankGameMode.h"

#include "TankGameInstance.h"
#include "ActorStart/ItemStart.h"
#include "ActorStart/TankStart.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "Controller/TankPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Tank/PlayerTank.h"
#include "Tank/TankItem.h"

void ABattleTankGameMode::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> tempATankItem;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATankItem::StaticClass(), tempATankItem);
	TankItemCount = tempATankItem.Num();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemStart::StaticClass(), OutItemStarts);
	ItemStartPointCount = OutItemStarts.Num();
	TeamAData = 0;
	TeamBData = 0;
}

void ABattleTankGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// FString healthMessage = FString::Printf(TEXT("%i remaining."), Tanks.Num());
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
}

void ABattleTankGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	auto TankPlayer = Cast<ATankPlayerController>(NewPlayer);
	auto Tank = Cast<ATank>(TankPlayer->GetPawn());
	if (!Tank) { return; }
	OutTanks.Add(Tank);
	if (TankMap.Contains(Tank->GetName()))TankMap[Tank->GetName()] = 0;
	else TankMap.Emplace(Tank->GetName(), 0);
	SortTankList();
}

void ABattleTankGameMode::StartPlay()
{
	Super::StartPlay();
	GetWorld()->GetTimerManager().SetTimer(SpawnItemTimer, this, &ABattleTankGameMode::SpawnTankItem, 5.f, true);
}

void ABattleTankGameMode::SpawnTankItem()
{
	if (TankItemCount >= ItemStartPointCount / 2) { return; }
	// 获得数据表中的数据
	UTankGameInstance* GameInstance = Cast<UTankGameInstance>(GetWorld()->GetGameInstance());
	UDataTable* ItemTable = GameInstance->GetItemDB();
	int32 Id = FMath::Rand() % 16 + 1;
	FTankItemMeta* ItemToRand = ItemTable->FindRow<FTankItemMeta>(FName(*FString::FromInt(Id)), FString(""));
	// 随机生成的点位
	auto Point = OutItemStarts[FMath::Rand() % ItemStartPointCount]->GetTransform();
	// 偏移量
	auto now = Point.GetLocation();
	float X = -100 + (200) * (rand() / (float)RAND_MAX);
	float Y = -100 + (200) * (rand() / (float)RAND_MAX);
	Point.SetLocation(now + FVector(X, Y, 0));
	ATankItem* TankItemRef = Cast<ATankItem>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ItemToRand->ItemClass, Point));
	if (TankItemRef)
	{
		TankItemRef->SetData(ItemToRand->ItemID, ItemToRand->Value + FMath::Rand() % 10);
	}
	UGameplayStatics::FinishSpawningActor(TankItemRef, Point);
	TankItemCount++;
}

void ABattleTankGameMode::HitGrade(ATank* A, ATank* B)
{
	if (A->GetTeam() != ETankTeam::Self && A->GetTeam() == B->GetTeam())
	{
		// 打中友军，自己扣分
		if (TankMap.Contains(A->GetName()))TankMap[A->GetName()] -= 10;
		if (A->GetTeam() == ETankTeam::TeamA)TeamAData -= 10;
		else if (A->GetTeam() == ETankTeam::TeamB) TeamBData -= 10;
	}
	else
	{
		if (TankMap.Contains(A->GetName()))TankMap[A->GetName()] += 5;
		if (A->GetTeam() == ETankTeam::TeamA)TeamAData += 5;
		else if (A->GetTeam() == ETankTeam::TeamB) TeamBData += 5;
		if (TankMap.Contains(B->GetName()))TankMap[B->GetName()] -= 1;
		if (B->GetTeam() == ETankTeam::TeamA)TeamAData -= 1;
		else if (B->GetTeam() == ETankTeam::TeamB) TeamBData -= 1;
	}
	SortTankList();
}

void ABattleTankGameMode::KillGrade(ATank* A, ATank* B)
{
	if (A->GetTeam() != ETankTeam::Self && A->GetTeam() == B->GetTeam())
	{
		// 杀死友军，自己扣分
		if (TankMap.Contains(A->GetName()))TankMap[A->GetName()] -= 50;
		if (A->GetTeam() == ETankTeam::TeamA)TeamAData -= 50;
		else if (A->GetTeam() == ETankTeam::TeamB) TeamBData -= 50;
	}
	else
	{
		if (TankMap.Contains(A->GetName()))TankMap[A->GetName()] += 20;
		if (A->GetTeam() == ETankTeam::TeamA)TeamAData += 20;
		else if (A->GetTeam() == ETankTeam::TeamB) TeamBData += 20;
	}
	SortTankList();
}

void ABattleTankGameMode::SortTankList()
{
	TankMap.ValueSort([](const int32& A, const int32& B)
		{
			return A > B;
		});
	for (auto OutTank : OutTanks)
	{
		auto player = Cast<APlayerTank>(OutTank);
		if (player)
		{
			player->GetTankListFString(TankMap);
			player->GetTankListint32(TankMap);
			player->GetTankTeamData(TeamAData, TeamBData);
		}
	}
}
