// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameState.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"
#include "Net/UnrealNetwork.h"

void ABlasterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterGameState, TopScoringPlayers);
}

void ABlasterGameState::UpdateTopScore(ABlasterPlayerState* BlasterPlayerState)
{
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(BlasterPlayerState);
		TopScore = BlasterPlayerState->GetScore();
	}

	else if (TopScore < BlasterPlayerState->GetScore())
	{
		TopScore = BlasterPlayerState->GetScore();
		TopScoringPlayers.Empty();
		TopScoringPlayers.Add(BlasterPlayerState);
	}

	else if (TopScore == BlasterPlayerState->GetScore())
	{
		TopScoringPlayers.AddUnique(BlasterPlayerState);
	}
}