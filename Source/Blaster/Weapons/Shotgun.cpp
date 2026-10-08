// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"

void AShotgun::Fire(const FVector& HitTarget)
{
	AWeapon::Fire(HitTarget);

	if (GetWeaponMesh())
	{
		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		if (!OwnerPawn) return;

		AController* OwnerController = OwnerPawn->GetController();
		const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
		if (MuzzleSocket)
		{
			FTransform MuzzleTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());

			const FVector Start = MuzzleTransform.GetLocation();
			FVector End = RandomScatter(Start, HitTarget);

		}
	}
}
