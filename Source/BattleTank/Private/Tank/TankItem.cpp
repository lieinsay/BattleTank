// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank/TankItem.h"

#include "TankGameInstance.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATankItem::ATankItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	ItemID = FName("Please enter an ID");

	Collision = CreateDefaultSubobject<USphereComponent>("PickupCollision");
	SetRootComponent(Collision);
	Collision->SetSphereRadius(64);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	Mesh->SetSimulatePhysics(true);
	Mesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

}

void ATankItem::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATankItem, ItemID);
	DOREPLIFETIME(ATankItem, Value);
}

void ATankItem::SetData(FName ItemIDToSet, int32 ValueToSet)
{
	OutID = 0;
	ItemID = ItemIDToSet;
	Value = ValueToSet;
}

void ATankItem::SetOutID(int32 OutIDToSet)
{
	OutID = OutIDToSet;
}

// Called when the game starts or when spawned
void ATankItem::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATankItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector NewLocation = GetActorLocation();
	FRotator NewRotation = GetActorRotation();
	float RunningTime = GetGameTimeSinceCreation();
	float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
	NewLocation.Z += DeltaHeight * 20;       //Scale our height by a factor of 20
	float DeltaRotation = DeltaTime * 60;    //Rotate by 20 degrees per second
	NewRotation.Yaw += DeltaRotation;
	SetActorLocationAndRotation(NewLocation, NewRotation);
}

FTankItemMeta ATankItem::GetTankItemData() const
{

	UTankGameInstance* GameInstance = Cast<UTankGameInstance>(GetWorld()->GetGameInstance());
	if (!GameInstance) { return {}; }
	UDataTable* ItemTable = GameInstance->GetItemDB();
	if (!ItemTable) { return {}; }
	FTankItemMeta* ItemToAdd = ItemTable->FindRow<FTankItemMeta>(ItemID, FString(""));
	if (ItemToAdd) {
		auto temp = *ItemToAdd;
		temp.ItemID = ItemID;
		temp.Value = Value;
		temp.OutID = OutID;
		return temp;
	}
	return {};
}

