// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ABlasterGameMode();

	void PlayerEliminated(class ABlasterCharacter* ElimmedCharacter,
		class ABlasterController* VictimController,
		ABlasterController* AttackerController
	);
	virtual void RequestRespawn(ABlasterCharacter* ElimmedCharacter,
		AController* ElimmedController);

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	float LevelStartingTime =0.f;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	float CountdownTime = 0.f;
};
