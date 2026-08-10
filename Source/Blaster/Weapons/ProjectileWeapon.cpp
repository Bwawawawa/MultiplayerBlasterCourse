// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

 
	if (!HasAuthority()) return;

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleSocket)
	{
		UE_LOG(LogTemp, Warning, TEXT("MuzzleSocket Reached"));
		FTransform MuzzleTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		FVector ToTarget = HitTarget - MuzzleTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		if (ProjectileClass && InstigatorPawn)
		{
			UE_LOG(LogTemp, Warning, TEXT("ProjectileClass Reached"));
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;

			UWorld* World = GetWorld();
			if (World)
			{
				UE_LOG(LogTemp, Warning, TEXT("SpawnLogic Reached"));
				World->SpawnActor<AProjectile>(
					ProjectileClass,
					MuzzleTransform.GetLocation(),
					TargetRotation,
					SpawnParams
				);
			}
		}
	}
}
