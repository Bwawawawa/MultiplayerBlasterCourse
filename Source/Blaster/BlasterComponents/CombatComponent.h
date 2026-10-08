// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blaster/Weapons/WeaponTypes.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000.f

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class ABlasterCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void EquipWeapon(class AWeapon* WeaponToEquip);

	void Reload();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

/// <summary>
/// Firing
/// </summary>

	void Fire();

	UFUNCTION()
	void FireActionFunction(bool bPressed);

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);
protected:
	virtual void BeginPlay() override;

	void SetAiming(bool NewbAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool NewbAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION(Server,Reliable)
	void ServerReload();

	void TraceUnderCrosshair(FHitResult& TracehitResult);

	void SetHUDCrosshairs(float DeltaTime);

	void HandleReloadLocally();

	int32 AmountToReload();
private:
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY()
	class ABlasterCharacter* Character;

	UPROPERTY()
	class ABlasterController* Controller;

	UPROPERTY()
	class ABlasterHUD* HUD;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool bFireButtonPressed;

	FVector HitTarget;

	/*
	   CROSSHAIR HUD
	*/

	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimingFactor;
	float CrosshairShootingFactor;
	/*
	*    WEAPON AIMING PROPERTIES
	* 
	    */

	float DefaultFOV;

	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = "Aiming")
	float ZoomedFOV;

	void InterpFOV(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = "Aiming")
	float ZoomInterpSpeed;

	/*
	   Automatic Fire
	*/

	FTimerHandle FireTimerHandle;
	bool bCanFire = true;

	void StartFireTimer();

	UFUNCTION()
	void FireTimerFinished();

	bool CanFire();

	//Carried Ammo Amount for the currently equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;

	UPROPERTY(EditAnywhere)
	int32 StartingRocketLauncherAmmo = 30;

	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 30;

	TMap<EWeaponType, int32> CarriedAmmoMap;

	void InitializeCarriedAmmo();

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	void UpdateAmmoValues();
public:	

};
