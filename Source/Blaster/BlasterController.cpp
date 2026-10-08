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
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/GameStates/BlasterGameState.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"


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
	SetHUDTime(DeltaTime);

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

void ABlasterController::SetHUDMatchCountdown(float CountdownTime, float DeltaTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid =
		BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->MatchCountdownText;
	if (bHUDValid)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;

		if (CountdownTime < 0.f)
		{
			BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}
		else if(CountdownTime < 30.f)
		{
			if (BlinkCountdown <= 0.f)
			{
				BlinkCountdown = MaxBlinkCountdown;
			}
			BlinkCountdown -= DeltaTime;
			FColor TColor(1, 1, 1, BlinkCountdown / MaxBlinkCountdown);
			BlasterHUD->CharacterOverlay->MatchCountdownText->SetColorAndOpacity(FSlateColor(TColor));
		}

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

		if (CountDown < 0.f)
		{
			BlasterHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}

		FString CountdownString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		BlasterHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownString));
	}
}

void ABlasterController::SetHUDTime(float DeltaTime)
{
	float CurrentTime = 0.f;

	if (MatchState == MatchState::WaitingToStart)
	{
		CurrentTime = WarmupTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::InProgress)
	{
		CurrentTime = MatchTime + WarmupTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::CooldownState)
	{
		CurrentTime = CoolDownTime + MatchTime + WarmupTime - GetServerTime() + LevelStartingTime;
	}

	if (HasAuthority())
	{
		BlasterGameMode = BlasterGameMode == nullptr ?
			Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;
		if (BlasterGameMode)
		{
			CurrentTime = BlasterGameMode->GetCountdownTime();
		}
	}

	float SecondsLeft = CurrentTime;
	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::CooldownState)
		{
			SetHUDAnnouncementCountDown(CurrentTime);
		}
		if(MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(CurrentTime, DeltaTime); 
		}
	}
	CountdownInt = SecondsLeft;
}

void ABlasterController::Server_GetMatchState_Implementation()
{
	BlasterGameMode = BlasterGameMode == nullptr ? 
		Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;

	if (BlasterGameMode)
	{
		MatchTime = BlasterGameMode->GModeMatchTime;
		LevelStartingTime = BlasterGameMode->LevelStartingTime;
		WarmupTime = BlasterGameMode->WarmupTime;
		CoolDownTime = BlasterGameMode->CoolDownTime;
		MatchState = BlasterGameMode->GetMatchState();

		Client_JoinMidGame(MatchTime,CoolDownTime, LevelStartingTime, WarmupTime, MatchState);
	}
}

void ABlasterController::Client_JoinMidGame_Implementation(float TotalMatchTime, float Cooldown,
	float TimeWhenLevelStarted, float GWarmupTime, FName CurrentMatchState)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	MatchTime = TotalMatchTime;
	LevelStartingTime = TimeWhenLevelStarted;
	WarmupTime = GWarmupTime;
	CoolDownTime = Cooldown; 
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

void ABlasterController::HandleMatchCooldown()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay) BlasterHUD->CharacterOverlay->RemoveFromParent();

		bool bHudValid = BlasterHUD->Announcement &&
			BlasterHUD->Announcement->AnnouncementText &&
			BlasterHUD->Announcement->InfoText;

		if (bHudValid)
		{
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Visible);

			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
			ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();

			if (BlasterGameState && BlasterPlayerState)
			{
				TArray<ABlasterPlayerState*> TopScoringPlayers = BlasterGameState->TopScoringPlayers;
				FString InfoTextString;

				if (TopScoringPlayers.Num() == 0.f)
				{
					InfoTextString = FString("Noone Scored");
				}
				if (TopScoringPlayers.Num() == 1)
				{
					InfoTextString = FString::Printf(TEXT("TopScoringPlayer: \n%s"), *TopScoringPlayers[0]->GetPlayerName());
				}
				else if (TopScoringPlayers.Num() > 1)
				{
					InfoTextString = FString("Multiple Top Scorers:\n");
					for (auto CurrentIndex : TopScoringPlayers)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *CurrentIndex->GetPlayerName()));
					}
				}

				BlasterHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}

			FString AnnouncementString = FString("New Match Starts in:");
			BlasterHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementString));
		}
	}
	ABlasterCharacter* CharRef = Cast<ABlasterCharacter>(GetPawn());
	if (CharRef)
	{
		CharRef->bDisableGameplay = true; 
		CharRef->GetCombatComponent()->FireActionFunction(false);
	}
}

void ABlasterController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	if (MatchState == MatchState::CooldownState)
	{
		HandleMatchCooldown();
	}
}

void ABlasterController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	if (MatchState == MatchState::CooldownState)
	{
		HandleMatchCooldown();
	}
}
