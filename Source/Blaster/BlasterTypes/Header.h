#pragma once

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	ETIP_TurningLeft UMETA(DisplayName = "Turning Left"),
	ETIP_TurningRight UMETA(DisplayName = "Turning Right"),
	ETIP_NotTurning UMETA(DisplayName = "Not Turning"),

	ETIP_MAX UMETA(DisplayName = "Default Max")
};