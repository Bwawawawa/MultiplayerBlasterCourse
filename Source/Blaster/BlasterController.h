// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterController.generated.h"

/**
 * 
 */

class UInputMappingContext;

UCLASS()
class BLASTER_API ABlasterController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetupInputComponent() override;

	void SetHUDHealth(float Health, float Maxhealth);

	void SetHUDScore(float Score);

	void SetDefeats(int32 Deaths);

	void SetHUDWeaponAmmo(int32 Ammo);

	void SetHUDCarriedAmmo(int32 Ammo);

	void SetHUDMatchCountdown(float CountdownTime, float DeltaTime);

	void SetHUDAnnouncementCountDown(float CountDown);

	virtual void OnPossess(APawn* InPawn) override;

	virtual float GetServerTime();

	virtual void ReceivedPlayer() override;

	void OnMatchStateSet(FName State);
protected:
	virtual void BeginPlay() override;

	void SetHUDTime(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void Server_GetMatchState();

	UFUNCTION(Client, Reliable)
	void Client_JoinMidGame(float TotalMatchTime, float Cooldown,
		float TimeWhenLevelStarted, float GWarmupTime, FName CurrentMatchState);

	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerRecievedClientRequest);

	float ClientServerTimeDelta = 0.f;

	UPROPERTY(EditAnywhere, Category = "Time")
	float TimeSyncFrequency = 6.0f;

	float TimeSyncRunningTime = 0.f;

	void CheckTimeSync(float DeltaTime);

	void PollInit();

	void HandleMatchHasStarted();

	void HandleMatchCooldown();
private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	UPROPERTY()
	class ABlasterGameMode* BlasterGameMode;

	UPROPERTY()
	class ABlasterHUD* BlasterHUD;

	UPROPERTY()
	float MatchTime = 0.f;

	UPROPERTY()
	float CoolDownTime = 0.f;

	UPROPERTY()
	float LevelStartingTime = 0.f;

	float WarmupTime = 0.f;

	UPROPERTY()
	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	bool bInitializeCharacterOverlay = false;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDefeats;

	float BlinkCountdown = 10.f;
	float MaxBlinkCountdown = 10.f;
};
