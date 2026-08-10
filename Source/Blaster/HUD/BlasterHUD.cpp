// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"
#include "CharacterOverlay.h"
#include "Announcement.h"

void ABlasterHUD::BeginPlay()
{
	Super::BeginPlay();
}

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		FVector2D ViewportCentre(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
		
		float CrosshairSpreadScaled = MaxCrosshairSpread * HUDPackage.CrosshairSpread;

		if (HUDPackage.CrosshairCentre)
		{
			FVector2D Spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.CrosshairCentre, ViewportCentre, Spread);
		}
		if (HUDPackage.CrosshairRight)
		{
			FVector2D Spread(CrosshairSpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairRight, ViewportCentre, Spread);
		}
		if (HUDPackage.CrosshairLeft)
		{
			FVector2D Spread(-CrosshairSpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairLeft, ViewportCentre, Spread);
		}
		if (HUDPackage.CrosshairUp)
		{
			FVector2D Spread(0.f, -CrosshairSpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairUp, ViewportCentre, Spread);
		}
		if (HUDPackage.CrosshairBottom)
		{
			FVector2D Spread(0.f, CrosshairSpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairBottom, ViewportCentre, Spread);
		}
	}
}

void ABlasterHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		if (CharacterOverlay)
		{
			CharacterOverlay->AddToViewport();
		}
	}
}

void ABlasterHUD::AddAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && AnnouncementClass)
	{
		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
		if (Announcement)
		{
			Announcement->AddToViewport();
		}
	}
}

void ABlasterHUD::DrawCrosshair(UTexture2D* TextureToDraw, FVector2D ViewportCentre, FVector2D Spread)
{
	const float TextureWidth = TextureToDraw->GetSizeX();
	const float TextureHeight = TextureToDraw->GetSizeY();

	const FVector2D TextureDrawPoint(
		ViewportCentre.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCentre.Y - (TextureHeight / 2.f) + Spread.Y
	);
	DrawTexture(
		TextureToDraw,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		FLinearColor::White
	);
}
