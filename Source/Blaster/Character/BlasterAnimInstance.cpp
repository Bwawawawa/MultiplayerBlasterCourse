// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaster/Weapons/Weapon.h"
#include "Blaster/BlasterTypes/CombatState.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	BlasterCharacterRef = Cast<ABlasterCharacter>(TryGetPawnOwner());
	if (BlasterCharacterRef)
	{
		MovementComp = BlasterCharacterRef->GetCharacterMovement();
	}
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (MovementComp)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(MovementComp->Velocity);
		bIsInAir = MovementComp->IsFalling();
		bIsAccelerating = MovementComp->GetCurrentAcceleration().Size() > 0.f ? true : false;
	}

	if (BlasterCharacterRef)
	{
		bWeaponEquipped = BlasterCharacterRef->IsWeaponEquipped();
		EquippedWeapon = BlasterCharacterRef->GetEquippedWeapon();
		bIsCrouched = BlasterCharacterRef->bIsCrouched;
		bAiming = BlasterCharacterRef->IsAiming();
		TurningInPlace = BlasterCharacterRef->GetTurningInPlace();
		bRotateRootBone = BlasterCharacterRef->ShouldRotateRootBone();
		bElimmed = BlasterCharacterRef->IsElimmed();

		// Offset Yaw for Strafing
		FRotator AimRotation = BlasterCharacterRef->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacterRef->GetVelocity());
		FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
		DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 15.f);
		YawOffset = DeltaRotation.Yaw;

		CharacterRotationLastFrame = CharacterRotation;
		CharacterRotation = BlasterCharacterRef->GetActorRotation();
		const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation,
			CharacterRotationLastFrame);
		const float Target = Delta.Yaw / DeltaSeconds;
		const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, 3.0f);
		Lean = FMath::Clamp(Interp, -90.0f, 90.0f);


		///AimOffste
		AO_Yaw = BlasterCharacterRef->GetAO_Yaw();
		AO_Pitch = BlasterCharacterRef->GetAO_Pitch();

		if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh()
			&& BlasterCharacterRef->GetMesh())
		{
			LeftHandTransform = EquippedWeapon->GetWeaponMesh()->
				GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
			FVector OutPosition;
			FRotator OutRotation;
			BlasterCharacterRef->GetMesh()->TransformToBoneSpace(FName("hand_r"),
				LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
			LeftHandTransform.SetLocation(OutPosition);
			LeftHandTransform.SetRotation(FQuat(OutRotation));

			if (BlasterCharacterRef->IsLocallyControlled())
			{
				bIsLocallyControlled = true;
				FTransform RightHandTransform = BlasterCharacterRef->GetMesh()->
					GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
				RightHandRotation = UKismetMathLibrary::FindLookAtRotation(
					RightHandTransform.GetLocation(),
					RightHandTransform.GetLocation() +
					(RightHandTransform.GetLocation() - BlasterCharacterRef->GetHitTarget())
				);
			}
		}
		bUseFabrik = BlasterCharacterRef->GetCombatState() != ECombatState::ECS_Reloading;
		bUseAimOffsets = BlasterCharacterRef->GetCombatState() != ECombatState::ECS_Reloading;
		bTransformRightHand = BlasterCharacterRef->GetCombatState() != ECombatState::ECS_Reloading;
	}
}
