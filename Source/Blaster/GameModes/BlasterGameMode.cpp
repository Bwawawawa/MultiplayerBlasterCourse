// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/BlasterController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"


ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterController* BlasterController = Cast<ABlasterController>(*It);
		if (BlasterController)
		{
			BlasterController->OnMatchStateSet(MatchState);
		}
	}
}

void ABlasterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter,
	ABlasterController* VictimController, ABlasterController* AttackerController)
{
	if (AttackerController && VictimController && ElimmedCharacter)
	{
		ElimmedCharacter->Elim();

		//Score RelatedStuff
		ABlasterPlayerState* AttackerPlayerState = AttackerController ?
			Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
		ABlasterPlayerState* VictimPlayerState = VictimController ?
			Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;
		if (AttackerPlayerState && VictimPlayerState && AttackerPlayerState != VictimPlayerState)
		{
			AttackerPlayerState->AddToScore(1.0f);
			VictimPlayerState->AddToDefeats(1);
		}
	}
}

void ABlasterGameMode::RequestRespawn(ABlasterCharacter* ElimmedCharacter,
	AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this,
			APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}
