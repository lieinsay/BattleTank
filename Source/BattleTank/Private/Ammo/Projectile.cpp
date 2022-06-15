// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo/Projectile.h"

#include "BattleTankGameMode.h"
#include "Component/TankArmorComponent.h"
#include "Component/TankHealthComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	CollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Collision Mesh"));
	SetRootComponent(CollisionMesh);
	CollisionMesh->SetNotifyRigidBodyCollision(true);
	CollisionMesh->SetVisibility(true);

	if (DefaultMesh.Succeeded())
	{
		CollisionMesh->SetStaticMesh(DefaultMesh.Object);
		CollisionMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		CollisionMesh->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
	}

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("Projectile Movement"));
	ProjectileMovement->SetIsReplicated(true);
	ProjectileMovement->bAutoActivate = false;

	ExplosionForce = CreateDefaultSubobject<URadialForceComponent>(FName("Explosion Force"));
	ExplosionForce->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultLaunchBlast(TEXT("/Game/VigilanteContent/Vehicles/East_SPG_2S3Akatsia/FX/FX_MuzzleFire_01_2S3Akatsia.FX_MuzzleFire_01_2S3Akatsia"));
	if (DefaultLaunchBlast.Succeeded())
	{
		LaunchBlast = DefaultLaunchBlast.Object;
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultImpactBlast(TEXT("/Game/Tank/V1/ImpactBlast.ImpactBlast"));
	if (DefaultImpactBlast.Succeeded())
	{
		ImpactBlast = DefaultImpactBlast.Object;
	}
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	CollisionMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::LaunchProjectile_Implementation()
{
	ProjectileMovement->SetVelocityInLocalSpace(FVector::ForwardVector * ProjectileSpeed);
	ProjectileMovement->Activate();
	UGameplayStatics::SpawnEmitterAtLocation(this, LaunchBlast, ExplosionForce->GetComponentLocation(), ExplosionForce->GetComponentRotation(), true, EPSCPoolMethod::AutoRelease);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	ExplosionForce->FireImpulse();
	CollisionMesh->DestroyPhysicsState();
	// 计算伤害
	auto tank = Cast<ATank>(OtherActor);
	if (tank)
	{
		auto tempDamage = ProjectileDamage - tank->GetTankTankArmor()->GetDefensive();
		ProjectileDamage = tempDamage > 0 ? tempDamage : ProjectileDamage / 4;
		auto nowHeath = tank->GetTankHealth()->GetCurrentHealth();
		// 将打击事件通知给GameMode
		auto GameMode = Cast<ABattleTankGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode && nowHeath > 0)
		{
			GameMode->HitGrade(TankReflection, tank);
		}
		if (GameMode && nowHeath > 0 && nowHeath <= ProjectileDamage)
		{
			GameMode->KillGrade(TankReflection, tank);
		}
	}
	UGameplayStatics::ApplyRadialDamage(
		this,
		ProjectileDamage,
		ExplosionForce->GetComponentLocation(),
		ExplosionForce->Radius, // for consistancy
		UDamageType::StaticClass(),
		TArray<AActor*>() // damage all actors
	);
	Destroy();
}

void AProjectile::Destroyed()
{
	FVector spawnLocation = GetActorLocation();
	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactBlast, spawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
}