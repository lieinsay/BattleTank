// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/TankInventoryComponent.h"

#include "BattleTankGameMode.h"
#include "TankGameInstance.h"
#include "Component/TankEquipmentComponent.h"
#include "Component/TankAimingComponent.h"
#include "Ammo/Projectile.h"
#include "Component/TankArmorComponent.h"
#include "Tank/Tank.h"
#include "Tank/TankItem.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UTankInventoryComponent::UTankInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	// ...
	Inventory = {};
	Nearby = {};
}

// Called when the game starts
void UTankInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	TankReflection = Cast<ATank>(GetOwner());
}

void UTankInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// 复制背包
	DOREPLIFETIME(UTankInventoryComponent, Inventory);
	DOREPLIFETIME(UTankInventoryComponent, Nearby);
}

ATank* UTankInventoryComponent::GetTank()
{
	if (IsValid(TankReflection))
	{
		return TankReflection;
	}
	TankReflection = Cast<ATank>(GetOwner());
	return Cast<ATank>(GetOwner());
}

// Called every frame
void UTankInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTankInventoryComponent::AddItemToInventory_Implementation(FTankItemMeta Item)
{
	Item.OutID = 0;
	auto FindItem = Inventory.FindByKey(Item);
	if (FindItem)
	{
		FindItem->Value += Item.Value;
	}
	else
	{
		Inventory.Add(Item);
	}
	DestroyItemToWorld(Item);
	GetTank()->OnInventoryUpdate();
}

void UTankInventoryComponent::RemoveItemToInventory_Implementation(FTankItemMeta Item)
{
	Inventory.Remove(Item);
	if (Item.Value > 0)
	{
		SpawnItemToWorld(Item);
	}
	GetTank()->OnInventoryUpdate();
}

void UTankInventoryComponent::AddItemToNearby(FTankItemMeta Item, ATankItem* TankItemRef)
{
	Item.OutID = Nearby.Num();
	Item.ItemRef = TankItemRef;
	TankItemRef->SetOutID(Nearby.Num());
	Nearby.Add(Item);
	if (IsValid(GetTank()))
		GetTank()->OnNearbyUpdate();
}

void UTankInventoryComponent::RemoveItemToNearby(FTankItemMeta Item)
{
	Nearby.Remove(Item);
	if (IsValid(GetTank()))
		GetTank()->OnNearbyUpdate();
}

void UTankInventoryComponent::DestroyItemToWorld_Implementation(FTankItemMeta Item)
{
	Item.ItemRef->Destroy();
	Nearby.Remove(Item);
	Cast<ABattleTankGameMode>(GetWorld()->GetAuthGameMode())->RemoveTankItem();
}

void UTankInventoryComponent::SpawnItemToWorld_Implementation(FTankItemMeta Item)
{
	ATankItem* TankItemRef = Cast<ATankItem>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, Item.ItemClass, TankReflection->GetTransform()));
	if (TankItemRef)
	{
		TankItemRef->SetData(Item.ItemID, Item.Value);
	}
	UGameplayStatics::FinishSpawningActor(TankItemRef, TankReflection->GetTransform());
	Cast<ABattleTankGameMode>(GetWorld()->GetAuthGameMode())->AddTankItem();
}

void UTankInventoryComponent::UseNowProjectileToInventory(FTankItemMeta Item)
{
	auto NowProjectile = Inventory.FindByKey(Item);
	if (NowProjectile)
	{
		NowProjectile->Value = Item.Value;
		if (NowProjectile->Value == 0)
		{
			GetTank()->GetTankEquipment()->UninstallNowProjectile(*NowProjectile);
			RemoveItemToInventory(*NowProjectile);
		}
		GetTank()->OnInventoryUpdate();
	}
}

void UTankInventoryComponent::UseNowArmorToInventory(FTankItemMeta Item)
{
	auto NowArmor = Inventory.FindByKey(Item);
	if (NowArmor)
	{
		NowArmor->Value -= 1;
		if (NowArmor->Value == 0)
		{
			GetTank()->GetTankEquipment()->ChangeNowArmor(Item);
			RemoveItemToInventory(Item);
		}
		GetTank()->OnInventoryUpdate();
	}
}

void UTankInventoryComponent::BeUsingToInventory(FTankItemMeta Item)
{
	UTankGameInstance* GameInstance = Cast<UTankGameInstance>(GetWorld()->GetGameInstance());
	UTankArmorComponent* TankArmor = GetTank()->GetTankTankArmor();
	auto UsingItem = Inventory.FindByKey(Item);
	if (GameInstance && TankArmor && UsingItem)
	{
		if (Item.ItemType == ETankItemType::Armor)
		{
			TankArmor->AddDefensive(GameInstance->GetDefensiveById(FCString::Atoi(*Item.ItemID.ToString())));
		}
		else if (Item.ItemType == ETankItemType::Projectile)
		{

		}
		UsingItem->IsUsing = true;
		GetTank()->OnInventoryUpdate();
	}
}

void UTankInventoryComponent::NoUsingToInventory(FTankItemMeta Item)
{
	UTankGameInstance* GameInstance = Cast<UTankGameInstance>(GetWorld()->GetGameInstance());
	UTankArmorComponent* TankArmor = GetTank()->GetTankTankArmor();
	auto UsingItem = Inventory.FindByKey(Item);
	if (GameInstance && TankArmor && UsingItem)
	{
		if (Item.ItemType == ETankItemType::Armor)
		{
			TankArmor->AddDefensive(-GameInstance->GetDefensiveById(FCString::Atoi(*Item.ItemID.ToString())));
		}
		else if (Item.ItemType == ETankItemType::Projectile)
		{

		}
		UsingItem->IsUsing = false;
		GetTank()->OnInventoryUpdate();
	}
}

void UTankInventoryComponent::OnRep_Inventory()
{
	if (IsValid(GetTank()))
		GetTank()->OnInventoryUpdate();
}

void UTankInventoryComponent::OnRep_Nearby()
{
	if (IsValid(GetTank()))
		GetTank()->OnNearbyUpdate();
}