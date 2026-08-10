// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "InitialState"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMax")
};


UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;

	void ShowPickupWidget(bool bShowWidget);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Fire(const FVector& HitTarget);

	virtual void OnRep_Owner() override;

	void Dropped();

	void SetHUDAmmo();

	void AddAmmo(int32 AmmoToAdd);
	/*
	   TEXTURES FOR CROSSHAIRS
	*/

	UPROPERTY(EditAnywhere, Category = "CrossHairs")
	class UTexture2D* CrosshairCentre;

	UPROPERTY(EditAnywhere, Category = "CrossHairs")
	UTexture2D* CrosshairRight;

	UPROPERTY(EditAnywhere, Category = "CrossHairs")
	UTexture2D* CrosshairLeft;

	UPROPERTY(EditAnywhere, Category = "CrossHairs")
	UTexture2D* CrosshairUp;

	UPROPERTY(EditAnywhere, Category = "CrossHairs")
	UTexture2D* CrosshairBottom;

	UPROPERTY(EditAnywhere)
	USoundBase* EquipSound;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


private:
	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* OverlapSphere;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* PickupWidget;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "WeaponProperties")
	EWeaponState WeaponState;

    UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	class UAnimationAsset* FireAnimation;

	UFUNCTION()
	void OnRep_WeaponState();

	/*
	    ZOOM WHILE AIMING
	*/

	UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	float ZoomInterpSpeed = 10.f;

	/*
		Automatic fire
	*/

	UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	float FireDelay = 0.15f;

	UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	bool bAutomatic = true;

	/*
	   Weapon Ammo
	*/

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo)
	int32 Ammo;

	void SpendRound();

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	UFUNCTION()
	void OnRep_Ammo();

	UPROPERTY()
	class ABlasterCharacter* BlasterOwnerCharacter;

	UPROPERTY()
	class ABlasterController* BlasterOwnerController;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

public:	

	void SetWeaponState(EWeaponState NewWeaponState);

	FORCEINLINE USphereComponent* GetOverlapSphere() const { return OverlapSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	FORCEINLINE float GetFireDelay() const { return FireDelay; }
	FORCEINLINE bool IsAutomaticFire() const { return bAutomatic; }
	bool IsEmpty();
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetWeaponAmmo() const { return Ammo; }
	FORCEINLINE int32 GetWeaponMagCapacity() const { return MagCapacity; }
};
