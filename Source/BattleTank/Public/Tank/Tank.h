// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Pawn.h"
#include "Tank.generated.h"


class UTankMovementComponent;
class UTankEquipmentComponent;
class UTankArmorComponent;
class UTankHealthComponent;
class UTankSkeletalMeshComponent;
class UTankInventoryComponent;
class UTankAimingComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTankDelegate);

UENUM(BlueprintType)
enum class ETankItemType : uint8
{
	Item,
	Projectile,
	Armor
};

UENUM(BlueprintType)
enum class ETankTeam : uint8
{
	Self,
	TeamA,
	TeamB
};

// 数据表中的类型定义，数据表如果采用了下面结构体的类型，数据表中就会显示他的所有数据
USTRUCT(BlueprintType)
struct FTankItemMeta : public FTableRowBase {

	GENERATED_BODY();

	FTankItemMeta();

	// 野生物品的ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank Item Meta")
		int32 OutID;

	// 物品的ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank Item Meta")
		FName ItemID;

	// 拾取类型对象类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank Item Meta")
		TSubclassOf<class ATankItem> ItemClass;

	// 拾取类型对象引用
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank Item Meta")
		ATankItem* ItemRef;

	// 对象名字
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank Item Meta")
		FText Name;

	// 对象类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank Item Meta")
		ETankItemType ItemType;

	// 对象的Actor的类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank Item Meta")
		TSubclassOf<AActor> ItemActor;

	// 对象的数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank Item Meta")
		int32 Value;

	// 储存对象图片信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank Item Meta")
		UTexture2D* Thumbnail;

	// 是否被使用或者装备
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank Item Meta")
		bool IsUsing;

	bool operator == (const FTankItemMeta& Item) const;
};

UCLASS()
class BATTLETANK_API ATank : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATank();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/*背包更新通知UI更新*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnInventoryUpdate();

	UFUNCTION(BlueprintImplementableEvent)
		void OnNearbyUpdate();

	/*
	 * Tank 公开的自己的死亡信息
	 */

	FTankDelegate OnDeath;


	UFUNCTION(Server, Reliable)
		void TankDeath();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* Corpse;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UParticleSystemComponent* CorpseParticle;

	FTimerHandle ReviveTimer;
	FTimerHandle CorpseTimer;

	UFUNCTION(Server, Reliable)
		void TankRevive();

	UFUNCTION(BlueprintImplementableEvent)
		void InstallUI();
	UFUNCTION(BlueprintImplementableEvent)
		void UnInstallUI();

	/* 阵营 */
	UPROPERTY(ReplicatedUsing = OnRep_Team)
		ETankTeam Team;
	UFUNCTION(BlueprintImplementableEvent)
		void OnRep_Team();
	UFUNCTION(BlueprintCallable)
		ETankTeam GetTeam() { return Team; }
	UFUNCTION(Server, Reliable, BlueprintCallable)
		void SetTeamSelf();
	UFUNCTION(Server, Reliable, BlueprintCallable)
		void SetTeamA();
	UFUNCTION(Server, Reliable, BlueprintCallable)
		void SetTeamB();
	/*
	 * 组件的Get
	 */

	UTankSkeletalMeshComponent* GetTankSkeletalMesh() { return TankSkeletalMesh; };
	UTankMovementComponent* GetTankMovement() { return TankMovement; };
	UTankAimingComponent* GetTankAiming() { return TankAiming; };
	UTankInventoryComponent* GetTankInventory() { return TankInventory; };
	UTankEquipmentComponent* GetTankEquipment() { return TankEquipment; };
	UTankHealthComponent* GetTankHealth() { return TankHealth; };
	UTankArmorComponent* GetTankTankArmor() { return TankArmor; };
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UTankSkeletalMeshComponent* TankSkeletalMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UTankMovementComponent* TankMovement;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UTankAimingComponent* TankAiming;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UTankInventoryComponent* TankInventory;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UTankEquipmentComponent* TankEquipment;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UTankHealthComponent* TankHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UTankArmorComponent* TankArmor;
};