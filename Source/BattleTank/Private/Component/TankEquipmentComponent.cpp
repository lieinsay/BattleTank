// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/TankEquipmentComponent.h"

#include "TankGameInstance.h"
#include "Ammo/Projectile.h"
#include "Component/TankAimingComponent.h"
#include "Component/TankArmorComponent.h"
#include "Component/TankInventoryComponent.h"
#include "Tank/Tank.h"

// Sets default values for this component's properties
UTankEquipmentComponent::UTankEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void UTankEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	GetWorld()->GetTimerManager().SetTimer(ArmorTimer, this, &UTankEquipmentComponent::UsingNowArmor, 1.f, true);
}


// Called every frame
void UTankEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

ATank* UTankEquipmentComponent::GetTank()
{
	if (IsValid(TankReflection))
	{
		return TankReflection;
	}
	TankReflection = Cast<ATank>(GetOwner());
	return Cast<ATank>(GetOwner());
}

int32 UTankEquipmentComponent::GetAmmoCount()
{
	UTankInventoryComponent* Inventory = GetTank()->GetTankInventory();
	if (!Inventory || NowProjectile.ItemID == FName("-1")) { return 0; }
	auto TankInventory = Inventory->GetInventory();
	return TankInventory.FindByKey(NowProjectile)->Value;
}

UTexture2D* UTankEquipmentComponent::GetProjectileImage()
{
	UTankInventoryComponent* Inventory = GetTank()->GetTankInventory();
	if (!Inventory || NowProjectile.ItemID == FName("-1")) { return nullptr; }
	auto TankInventory = Inventory->GetInventory();
	return TankInventory.FindByKey(NowProjectile)->Thumbnail;
}

void UTankEquipmentComponent::InstallNowProjectile(FTankItemMeta Item)
{
	if (Item.IsUsing) { return; }
	UTankInventoryComponent* Inventory = GetTank()->GetTankInventory();
	UTankAimingComponent* Aiming = GetTank()->GetTankAiming();
	if (!Inventory || !Aiming || !Inventory->GetInventory().FindByKey(Item)) { return; }
	Inventory->BeUsingToInventory(Item);
	Inventory->NoUsingToInventory(NowProjectile);
	NowProjectile = *Inventory->GetInventory().FindByKey(Item);
	Aiming->SetProjectileBlueprint(NowProjectile.ItemActor);
	Aiming->SetSpeed(Cast<UTankGameInstance>(GetWorld()->GetGameInstance())->GetProjectileSpeedById(FCString::Atoi(*NowProjectile.ItemID.ToString())));
	Aiming->PlayerLoading();
}

void UTankEquipmentComponent::UninstallNowProjectile(FTankItemMeta Item)
{
	if (!Item.IsUsing) { return; }
	UTankInventoryComponent* Inventory = GetTank()->GetTankInventory();
	UTankAimingComponent* Aiming = GetTank()->GetTankAiming();
	if (!Inventory || !Aiming || !Inventory->GetInventory().FindByKey(Item)) { return; }
	Inventory->NoUsingToInventory(Item);
	NowProjectile = FTankItemMeta();
	Aiming->SetProjectileBlueprint(nullptr);
}

void UTankEquipmentComponent::UseNowProjectile()
{
	UTankInventoryComponent* Inventory = GetTank()->GetTankInventory();
	if (!Inventory || NowProjectile.ItemID == FName("-1")) { return; }
	if (Inventory->GetInventory().FindByKey(NowProjectile))
	{
		NowProjectile.Value -= 1;
		Inventory->UseNowProjectileToInventory(NowProjectile);
	}
}

void UTankEquipmentComponent::ChangeNowArmor(FTankItemMeta Item)
{
	GetWorld()->GetTimerManager().ClearTimer(ArmorTimer);
	UTankInventoryComponent* Inventory = GetTank()->GetTankInventory();
	if (!Inventory) { return; }
	auto find = NowArmor.FindByKey(Item);
	if (find)
	{
		Inventory->NoUsingToInventory(Item);
		NowArmor.Remove(Item);
	}
	else if (NowArmor.Num() == 2)
	{
		auto removeItem = NowArmor.GetData()[0];
		Inventory->NoUsingToInventory(removeItem);
		Inventory->BeUsingToInventory(Item);
		NowArmor.Remove(removeItem);
		NowArmor.Add(Item);
	}
	else
	{
		Inventory->BeUsingToInventory(Item);
		NowArmor.Add(Item);
	}
	GetTank()->OnInventoryUpdate();
	GetWorld()->GetTimerManager().SetTimer(ArmorTimer, this, &UTankEquipmentComponent::UsingNowArmor, 1.f, true);
}

void UTankEquipmentComponent::UsingNowArmor()
{
	UTankInventoryComponent* Inventory = GetTank()->GetTankInventory();
	if (!Inventory || NowArmor.Num() == 0) { return; }
	for (int i = 0; i < NowArmor.Num(); i++)
	{
		if (Inventory->GetInventory().FindByKey(NowArmor[i]))
		{
			Inventory->UseNowArmorToInventory(NowArmor[i]);
			NowArmor[i].Value = Inventory->GetInventory().FindByKey(NowArmor[i])->Value;
		}
	}
}
