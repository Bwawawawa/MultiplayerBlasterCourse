// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual void OnRep_Score() override;
	UFUNCTION()
	virtual void OnRep_Defeats();

	void UpdateHUDScore();
	void AddToScore(float ScoreToAdd);

	void UpdateDefeats();
	void AddToDefeats(int32 DefeatsToAdd);

private:
	UPROPERTY()
	class ABlasterCharacter* BlasterCharacter;

	UPROPERTY()
	class ABlasterController* BlasterController;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;
};
