
#pragma once

UENUM(BlueprintType)
enum class EActionCode : uint8
{
	SHOOT  UMETA(DisplayName = "Shooting"),
	RELOAD UMETA(DisplayName = "Reloading"),
	DISTRACT UMETA(DisplayName = "Distracting"),
	FLANK UMETA(DisplayName = "Flanking"),
	VAULT UMETA(DisplayName = "Vaulting"),
	HOOK UMETA(DisplayName = "Hooking"),
	GADGET UMETA(DisplayName = "Using Gadget"),
	ENEMY_COVERING UMETA(DisplayName = "Covering (Enemy)"),
	ENEMY_ABOUT_TO_COVER UMETA(DisplayName = "About to Cover (Enemy)"),
	PLAYER_COVERING UMETA(DisplayName = "Covering (Player)"),
	PLAYER_ABOUT_TO_COVER UMETA(DisplayName = "About to Cover (Player)"),
	BREAK_SHIELD UMETA(DisplayName = "Break Shield"),
	THROW_GRENADE UMETA(DisplayName = "Throwing Grenade"),
	BOUNCE_GRENADE UMETA(DisplayName = "Bounce Grenade"),
	ENEMY_DEFEAT UMETA(DisplayName = "Defeated (Enemy)"),
	OTHER UMETA(DisplayName = "Other")
};