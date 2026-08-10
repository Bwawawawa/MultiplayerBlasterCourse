// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystemInstance.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "NiagaraSystemInstanceController.h"
#include "Blaster/Weapons/RocketMovementComponent.h"

AProjectileRocket::AProjectileRocket()
{
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
	RocketMesh->SetupAttachment(GetRootComponent());
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	NiagaraProjectile = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraProjectile"));
	NiagaraProjectile->SetupAttachment(GetRootComponent());
	NiagaraProjectile->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RocketMovement = CreateDefaultSubobject<URocketMovementComponent>(TEXT("RocketMovement"));
	RocketMovement->bRotationFollowsVelocity = true;
	RocketMovement->SetIsReplicated(true);
	RocketMovement->InitialSpeed = 1500.f;
	RocketMovement->MaxSpeed = 1500.f;
}

void AProjectileRocket::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectileRocket::OnHit);
	}

	if (TrailSystem)
	{
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailSystem,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}

	if (ProjectileLoop && LoopingSoundAttenuation)
	{
		ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(
			ProjectileLoop,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			EAttachLocation::KeepWorldPosition,
			false,
			1.0f,
			1.0f,
			0.f,
			LoopingSoundAttenuation,
			(USoundConcurrency*)nullptr,
			false);
	}
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetOwner())
	{
		return;
	}

	APawn* FiringPawn = GetInstigator();
	UE_LOG(LogTemp, Warning, TEXT("ReachedOnHit : Rocket"));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.AddUnique(this);

	AController* FiringController = FiringPawn->GetController();
	if (FiringController && HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("FiringControllerValid: Rocket"));
		UGameplayStatics::ApplyRadialDamageWithFalloff(
			this,  // WorldContextObject
			Damage, // BaseDamage
			10.f,   // BaseDamage
			GetActorLocation(), // Origin
			200.f,  // InnerRadius
			500.f,  // OuterRadius
			1.f,    // FallOff
			UDamageType::StaticClass(),
			ActorsToIgnore,  // IgnoreActors
			this,
			FiringController  // InstigatorController
		);
	}

	GetWorldTimerManager().SetTimer(
		DestroyTimer,
		this,
		&AProjectileRocket::DestroytimerFinished,
		DestroyTime
	);

	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ImpactParticles,
			GetActorTransform()
		);
	}

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ImpactSound,
			GetActorLocation()
		);
	}

	if (TrailSystemComponent && TrailSystemComponent->GetSystemInstanceController())
	{
		TrailSystemComponent->GetSystemInstanceController()->Deactivate();
	}

	if (RocketMesh)
	{
		RocketMesh->SetVisibility(false);
	}

	if (CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
	{
		ProjectileLoopComponent->Stop();
	}

}

void AProjectileRocket::DestroytimerFinished()
{
	Destroy();
}


void AProjectileRocket::Destroyed()
{

}
