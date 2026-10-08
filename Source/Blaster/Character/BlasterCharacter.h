// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Blaster/Blastertypes/Header.h"
#include "Blaster/Blastertypes/CombatState.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	void PlayFireMontage(bool bAiming);

	void PlayReloadMontage();

	virtual void OnRep_ReplicatedMovement() override;

	void Elim();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

	void PlayElimMontage();

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

protected:
	virtual void BeginPlay() override;
	



	/// <summary>
	/// 
	///   InputRelatedStuff 
	/// 
	/// </summary>
	
	/// *******MOVEACTION*********
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* MoveAction;

	UFUNCTION()
	void MoveFunction(const FInputActionValue& ActionValue);


	/// *******LookAction*********
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	UFUNCTION()
	void LookFunction(const FInputActionValue& ActionValue);


	/// *******JUMPACTION*********
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	//jump overrides
	virtual void Jump() override;
	virtual void StopJumping() override;

	//********EQUIPACTION*********
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* EquipAction;

	UFUNCTION()
	void EquipActionFunction();

	//********CrouchAction*********
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CrouchAction;

	UFUNCTION()
	void StartCrouching();

	UFUNCTION()
	void StopCrouching();
     
	//********AimAction*********
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AimAction;

	UFUNCTION()
	void StartAiming();

	UFUNCTION()
	void StopAiming();

	//******FireAction********
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* FireAction;
	
	UFUNCTION()
	void FireButtonPressed();

	UFUNCTION()
	void FireButtonReleased();

	//******FireAction********
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ReloadAction;
	
	UFUNCTION()
	void ReloadButtonPressed();

	//*************GANEPLAYRELATEDFUNCTIONS***********
	void LocalNetRoleDiplayFunction();

	void CalculateAO_Pitch();

	void AimOffset(float DeltaSeconds);

	void SimProxiesTurn();

	void PlayHitReactMontage();

	UFUNCTION()
	void RecieveDamage(AActor* DamagedActor, float Damage,
		const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void UpdateHUDHealth();

	//poll for any relevant classes and initialize our HUD
	void PollToInit();

	void RotateInPlace(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UFUNCTION(Server, Reliable)
	void ServerEquipFunction();

	/*
	   ********AnimationMontages********
	*/

	UPROPERTY(EditAnywhere, Category = "Combat")
	class UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	class UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	class UAnimMontage* ElimMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	class UAnimMontage* ReloadMontage;

	void HideCharacterifCameraClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;

	/*
	   CharacterHEalth
	*/

	UPROPERTY(EditAnywhere, Category = "PlayerStats")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_ReplicatedHealth, Category = "PlayerStats")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_ReplicatedHealth();

	bool bElimmed =false;

	FTimerHandle ElimTimer;

	UFUNCTION()
	void ElimTimerFinished();

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 2.5f;

	UPROPERTY()
	class ABlasterController* BlasterPlayerController;

	UPROPERTY()
	class ABlasterPlayerState* BlasterPlayerState;
public:	
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	AWeapon* GetEquippedWeapon();
	FVector GetHitTarget();
	FORCEINLINE UCameraComponent* GetCameraComponent() { return Camera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	ECombatState GetCombatState() const;
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UCombatComponent* GetCombatComponent() { return Combat; }
};
