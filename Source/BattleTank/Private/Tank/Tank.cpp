// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank/Tank.h"

#include "BattleTankGameMode.h"
#include "Ammo/Projectile.h"
#include "Component/TankAimingComponent.h"
#include "Component/TankArmorComponent.h"
#include "Component/TankEquipmentComponent.h"
#include "Component/TankHealthComponent.h"
#include "Component/TankInventoryComponent.h"
#include "Component/TankSkeletalMeshComponent.h"
#include "Component/TankMovementComponent.h"
#include "Controller/TankPlayerController.h"
#include "Controller/TankAIController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "Tank/TankItem.h"

FTankItemMeta::FTankItemMeta()
{
	OutID = 0;
	ItemID = FName("-1");
	Name = FText::FromString("Item");
	ItemType = ETankItemType::Item;
	Value = 0;
	ItemRef = nullptr;
	Thumbnail = nullptr;
	IsUsing = false;
}

bool FTankItemMeta::operator==(const FTankItemMeta& Item) const
{
	// 重载等于号，如果他们ID相同，就说明他们两个是属于同一种类型。
	if (ItemID == Item.ItemID && OutID == Item.OutID)
	{
		return true;
	}
	return false;
}

// Sets default values
ATank::ATank()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TankSkeletalMesh = CreateDefaultSubobject<UTankSkeletalMeshComponent>(TEXT("TankSkeletalMesh"));
	SetRootComponent(TankSkeletalMesh);
	TankMovement = CreateDefaultSubobject<UTankMovementComponent>(TEXT("TankMovement"));
	TankMovement->SetUpdatedComponent(TankSkeletalMesh);

	TankAiming = CreateDefaultSubobject<UTankAimingComponent>(TEXT("TankAiming"));
	TankInventory = CreateDefaultSubobject<UTankInventoryComponent>(TEXT("TankInventory"));
	TankEquipment = CreateDefaultSubobject<UTankEquipmentComponent>(TEXT("TankEquipment"));
	TankHealth = CreateDefaultSubobject<UTankHealthComponent>(TEXT("TankHealth"));
	TankArmor = CreateDefaultSubobject<UTankArmorComponent>(TEXT("TankArmor"));

	Corpse = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TankDead"));
	Corpse->SetupAttachment(TankSkeletalMesh);
	Corpse->SetVisibility(false, true);
	CorpseParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DeadParticle"));
	CorpseParticle->SetupAttachment(Corpse);
	Team = ETankTeam::Self;
}

void ATank::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATank, Team);
}

void ATank::SetTeamB_Implementation()
{
	Team = ETankTeam::TeamB;
}

void ATank::SetTeamA_Implementation()
{
	Team = ETankTeam::TeamA;
}

void ATank::SetTeamSelf_Implementation()
{
	Team = ETankTeam::Self;
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ATank::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (TankHealth->GetCurrentHealth() <= 0) { return 0; }
	float DamageToApply = FMath::Clamp<float>(DamageAmount, 0, TankHealth->GetCurrentHealth());
	TankHealth->SetCurrentHealth(TankHealth->GetCurrentHealth() - DamageToApply);
	if (TankHealth->GetCurrentHealth() <= 0)
	{
		TankDeath();
	}
	return DamageToApply;
}

void ATank::TankDeath_Implementation()
{
	Corpse->SetVisibility(true, true);
	TankSkeletalMesh->SetVisibility(false, false);
	GetWorld()->GetTimerManager().SetTimer(ReviveTimer, this, &ATank::TankRevive, 5.f, false);
}

void ATank::TankRevive_Implementation()
{
	TankSkeletalMesh->SetVisibility(true, false);
	Corpse->SetVisibility(false, true);
	TankHealth->SetCurrentHealth(TankHealth->GetMaxHealth());
	auto GameMode = Cast<ABattleTankGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		TeleportTo(GameMode->ChoosePlayerStart(GetController())->GetActorLocation(), GameMode->ChoosePlayerStart(GetController())->GetActorRotation());
	}
}
