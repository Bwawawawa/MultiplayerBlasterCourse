// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire(const FVector& HitTarget) override;

protected:
	FVector RandomScatter(const FVector& Start, const FVector& Target);

private:
	UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	class UParticleSystem* ImpactParticle;

	UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	class UParticleSystem* BeamParticle;

	UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	float Damage = 5.f;

	UPROPERTY(EditAnywhere, Category = "BulletScatter")
	float ScatterSphereDistance = 800.f;

	UPROPERTY(EditAnywhere, Category = "BulletScatter")
	float ScatterSphereRadius = 30.f;
};
