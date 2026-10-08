// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Particles/ParticleSystemComponent.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (GetWeaponMesh())
	{
		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		if (!OwnerPawn) return;

		AController* OwnerController = OwnerPawn->GetController();
		const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
		if (MuzzleSocket)
		{
			FTransform MuzzleTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());

			FVector Start = MuzzleTransform.GetLocation();
			FVector End = Start + (HitTarget - Start) * 1.25f;
			FHitResult FireHit;

			UWorld* ThisWorld = GetWorld();
			if (ThisWorld)
			{
				ThisWorld->LineTraceSingleByChannel(
					FireHit,
					Start,
					End,
					ECollisionChannel::ECC_Visibility
				);

				FVector BeamEnd = End;

				if (FireHit.bBlockingHit)
				{
					BeamEnd = FireHit.ImpactPoint;

					ABlasterCharacter* HitCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
					if (HasAuthority() && HitCharacter && OwnerController)
					{
					 UGameplayStatics::ApplyDamage(
						HitCharacter,
						Damage,
						OwnerController,
						this,
						UDamageType::StaticClass()
						);
					}

					UGameplayStatics::SpawnEmitterAtLocation(
						ThisWorld,
						ImpactParticle,
						FireHit.ImpactPoint,
						FireHit.ImpactNormal.Rotation()
					);
				}

				if (BeamParticle)
				{
					UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
						ThisWorld,
						BeamParticle,
						MuzzleTransform
					);

					if (Beam)
					{
						Beam->SetVectorParameter(FName("Target"), BeamEnd);
					}
				}
			}
		}
	}
}

FVector AHitScanWeapon::RandomScatter(const FVector& Start, const FVector& Target)
{
	FVector End = (Target - Start).GetSafeNormal();
	FVector EndLoc = Start + End * ScatterSphereDistance;

	DrawDebugSphere(
		GetWorld(),
		EndLoc,
		ScatterSphereDistance,
		20,
		FColor::Red,
		true
	);

	return FVector();
}
