// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Components/Progressbar.h"
#include "Components/Textblock.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/GameModes/BlasterGameMode.h"
#include "Blaster/HUD/Announcement.h"
#include "Kismet/GameplayStatics.h"

void ABlasterController::SetupInputComponent()
{
	if (IsLocalPlayerController() && GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* EILPS =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				int32 Priority = 0;
				EILPS->AddMappingContext(CurrentContext, Priority);
				Priority++;
			}
		}
	}
}

void ABlasterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);
	if (BlasterCharacter)
	{
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
	}
}


void ABlasterController::BeginPlay()
{
	Super::BeginPlay();
	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	Server_GetMatchState();
}

void ABlasterController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterController, MatchState);

}

void ABlasterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetHUDTime();

	CheckTimeSync(DeltaTime);
	PollInit();
}

void ABlasterController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void ABlasterController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (BlasterHUD && BlasterHUD->CharacterOverlay)
		{
			CharacterOverlay = BlasterHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetDefeats(HUDDefeats);
			}
		}
	}
}

void ABlasterController::SetHUDHealth(float Health, float Maxhealth)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid =
		BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HealthBar &&
		BlasterHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		const float HealthPercent = Health / Maxhealth;
		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);

		FString HealthString = FString::Printf(TEXT("%d/%d"),
			FMath::CeilToInt32(Health), FMath::CeilToInt32(Maxhealth));
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthString));
	}
	else
	{
		HUDHealth = Health;
		HUDMaxHealth = Maxhealth;
	}
}

void ABlasterController::SetHUDScore(float Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid =
		BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->ScoreAmount;
	if (bHUDValid)
	{
		FString ScoreString = FString::Printf(TEXT("%d"), FMath::FloorToInt32(Score));
		BlasterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreString));
	}
	else
	{
		HUDScore = Score;
		bInitializeCharacterOverlay = true;
	}
}

void ABlasterController::SetDefeats(int32 Defeats)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid =
		BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->DefeatsAmount;
	if (bHUDValid)
	{
		FString DefeatsString = FString::Printf(TEXT("%d"), Defeats);
		BlasterHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsString));
	}
	else
	{
		HUDDefeats = Defeats;
		bInitializeCharacterOverlay = true;
	}
}

void ABlasterController::SetHUDWeaponAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid =
		BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoAmountString = FString::Printf(TEXT("%d"), Ammo);
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoAmountString));
	}
}

void ABlasterController::SetHUDCarriedAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid =
		BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->CarriedAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoAmountString = FString::Printf(TEXT("%d"), Ammo);
		BlasterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoAmountString));
	}
}

void ABlasterController::SetHUDMatchCountdown(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid =
		BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->MatchCountdownText;
	if (bHUDValid)
	{
		UE_LOG(LogTemp, Warning, TEXT("MatchCountdownValid"));
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;

		FString CountdownString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownString));
	}
}

void ABlasterController::SetHUDAnnouncementCountDown(float CountDown)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid =
		BlasterHUD &&
		BlasterHUD->Announcement &&
		BlasterHUD->Announcement->WarmupTime;
	if (bHUDValid)
	{
		int32 Minutes = FMath::FloorToInt(CountDown / 60.f);
		int32 Seconds = CountDown - Minutes * 60;

		FString CountdownString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		UE_LOG(LogTemp, Warning, TEXT("%d"), Seconds);
		BlasterHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownString));
	}
}

void ABlasterController::SetHUDTime()
{
	float CurrentTime = 0.f;

	if (MatchState == MatchState::WaitingToStart)
	{
		CurrentTime =
			WarmupTime - GetServerTime() + LevelStartingTime;
		UE_LOG(LogTemp, Warning, TEXT("%f"), CurrentTime);
	}
	else if (MatchState == MatchState::InProgress)
	{
		UE_LOG(LogTemp, Warning, TEXT("SettingMatchCountDownTimer"));
		CurrentTime =
			MatchTime + WarmupTime - GetServerTime() + LevelStartingTime;
	}

	float SecondsLeft = CurrentTime;
	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart)
		{
			SetHUDAnnouncementCountDown(CurrentTime);
		}
		if(MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(CurrentTime);
		}
	}
	CountdownInt = SecondsLeft;
}

void ABlasterController::Server_GetMatchState_Implementation()
{
	ABlasterGameMode* BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));

	if (BlasterGameMode)
	{
		MatchTime = BlasterGameMode->GModeMatchTime;
		LevelStartingTime = BlasterGameMode->LevelStartingTime;
		WarmupTime = BlasterGameMode->WarmupTime;
		MatchState = BlasterGameMode->GetMatchState();

		Client_JoinMidGame(MatchTime, LevelStartingTime, WarmupTime, MatchState);
	}
}

void ABlasterController::Client_JoinMidGame_Implementation(float TotalMatchTime,
	float TimeWhenLevelStarted, float GWarmupTime, FName CurrentMatchState)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	MatchTime = TotalMatchTime;
	LevelStartingTime = TimeWhenLevelStarted;
	WarmupTime = GWarmupTime;
	MatchState = CurrentMatchState;

	OnMatchStateSet(MatchState);

	if (BlasterHUD && MatchState == MatchState::WaitingToStart) BlasterHUD->AddAnnouncement();
}

void ABlasterController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeofReciept = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeofReciept);
}

void ABlasterController::ClientReportServerTime_Implementation(float TimeOfClientRequest,
	float TimeServerRecievedClientRequest)
{
	float RoundTrip = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerRecievedClientRequest + RoundTrip * 0.5f;

	ClientServerTimeDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float ABlasterController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + ClientServerTimeDelta;
}

void ABlasterController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABlasterController::HandleMatchHasStarted()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		if(BlasterHUD->CharacterOverlay == nullptr) BlasterHUD->AddCharacterOverlay();
		if (BlasterHUD->Announcement)
		{
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ABlasterController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void ABlasterController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}
