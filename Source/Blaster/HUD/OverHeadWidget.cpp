// Fill out your copyright notice in the Description page of Project Settings.


#include "OverHeadWidget.h"
#include "Components/TextBlock.h"


void UOverHeadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
		UE_LOG(LogTemp, Warning, TEXT("SetTextFired"));
	}
}

void UOverHeadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	if (InPawn && InPawn->GetController() && DisplayText)
	{
		ENetRole PawnNetRole = InPawn->GetController()->GetLocalRole();
		FString Role;

		switch (PawnNetRole)
		{
		case ENetRole::ROLE_Authority:
			Role = FString("Authority");
			break;
		case ENetRole::ROLE_AutonomousProxy:
			Role = FString("AutonomousProxy");
			break;
		case ENetRole::ROLE_SimulatedProxy:
			Role = FString("SimulatedProxy");
			break;
		case ENetRole::ROLE_None:
			Role = FString("Nothing");
			break;

		default:
			break;
		}

		FString LocalNetRole = FString::Printf(TEXT("LocalRole: %s"), *Role);
		SetDisplayText(LocalNetRole);
		UE_LOG(LogTemp, Warning, TEXT("ShowPlayernetRoleFired"));
	}
}
