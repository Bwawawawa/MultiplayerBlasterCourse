// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/BlasterController.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABlasterPlayerState, Defeats);
}


void ABlasterPlayerState::UpdateHUDScore()
{

	BlasterCharacter = BlasterCharacter == nullptr ?
		Cast<ABlasterCharacter>(GetPawn()) : BlasterCharacter;
	if (BlasterCharacter)
	{
		BlasterController = BlasterController == nullptr ?
			Cast<ABlasterController>(BlasterCharacter->GetController()) : BlasterController;
		if (BlasterController)
		{
			BlasterController->SetHUDScore(GetScore());
		}
	}
}

void ABlasterPlayerState::UpdateDefeats()
{
	BlasterCharacter = BlasterCharacter == nullptr ?
		Cast<ABlasterCharacter>(GetPawn()) : BlasterCharacter;
	if (BlasterCharacter)
	{
		BlasterController = BlasterController == nullptr ?
			Cast<ABlasterController>(BlasterCharacter->GetController()) : BlasterController;
		if (BlasterController)
		{
			BlasterController->SetDefeats(Defeats);
		}
	}
}

void ABlasterPlayerState::AddToScore(float ScoreToAdd)
{
	SetScore(GetScore() + ScoreToAdd);
	UpdateHUDScore();
}

void ABlasterPlayerState::AddToDefeats(int32 DefeatsToAdd)
{
	Defeats += DefeatsToAdd;
	UpdateDefeats();
}

void ABlasterPlayerState::OnRep_Score()
{
	UpdateHUDScore();
}

void ABlasterPlayerState::OnRep_Defeats()
{
	UpdateDefeats();
}
