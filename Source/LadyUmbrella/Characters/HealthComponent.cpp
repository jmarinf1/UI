#include "HealthComponent.h"

#include "LadyUmbrella/Util/Logger.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "Player/PlayerCharacter.h"

#pragma region BASIC FUNCTIONS

UHealthComponent::UHealthComponent()
{
	// Set default values
	PrimaryComponentTick.bCanEverTick = true;
	CurrentHealth		 = 100;
	MaxHealth			 = 100;
	bCanRegenerate		 = false;
	RegenerationSpeed	 = .2f;
	RegenerationCooldown = 3.f;
	RegenerationValue	 = 1;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	StartRegenHealth();	
}

#pragma endregion 

#pragma region PRIVATE FUNCTIONS

void UHealthComponent::RegenerateHealth()
{
	if (GetHealth() < GetMaxHealth())
	{
		AddHealth(RegenerationValue);
	}
	else
	{
		StopRegenHealth();
	}
}

void UHealthComponent::StartRegenHealth()
{
	// if (GetWorld()->GetTimerManager().IsTimerActive(RegenTimerHandle))
	// {
	// 	GetWorld()->GetTimerManager().ClearTimer(RegenTimerHandle);
	// }
	if (bCanRegenerate)
	{
		GetWorld()->GetTimerManager().SetTimer
		(
			RegenTimerHandle,
			this,
			&UHealthComponent::RegenerateHealth,
			RegenerationSpeed,
			true,
			RegenerationCooldown
		);
	}
}

void UHealthComponent::StopRegenHealth()
{
	if (bCanRegenerate)
	{
		GetWorld()->GetTimerManager().ClearTimer(RegenTimerHandle);
	}
}

void UHealthComponent::OnDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("OnDeath"));
}

#pragma endregion

#pragma region PUBLIC FUNCTIONS

void UHealthComponent::SetHealth(int NewHealth)
{
	// Clamping the NewHealth.
	NewHealth = FMath::Clamp(NewHealth, 0, MaxHealth);

	// If after the clamp we are not actually changing our life amount, we return.
	if (NewHealth == CurrentHealth)
	{
		return;
	}

	int OldHealth = CurrentHealth;
	CurrentHealth = NewHealth;

	// Delegate broadcast when the health ammount changes.
	if (OldHealth > CurrentHealth)
	{
		OnHealthAmmountChanged.Broadcast(GetHealth(), true);
	}
	else
	{
		OnHealthAmmountChanged.Broadcast(GetHealth(), false);
	}

	// Delegate broadcast when the owner dies.
	if (IsDead())
	{
		OnDeathDelegate.Broadcast();
	}
}

void UHealthComponent::AddHealth(int HealthToAdd)
{
	if (HealthToAdd < 0)
	{
		HealthToAdd = -HealthToAdd;
	}
	
	SetHealth(CurrentHealth + HealthToAdd);
}

void UHealthComponent::SubtractHealth(int HealthToSubtract)
{
	if (HealthToSubtract < 0)
	{
		HealthToSubtract = -HealthToSubtract;
	}
	
	SetHealth(CurrentHealth - HealthToSubtract);

	// Stopping the regeneration of health.
	StopRegenHealth();

	// And setting the timer to start regenerating again soon.
	if (bCanRegenerate)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(RegenDelayHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(RegenDelayHandle);
		}
		GetWorld()->GetTimerManager().SetTimer
		(
			RegenDelayHandle,
			this,
			&UHealthComponent::StartRegenHealth,
			RegenerationSpeed,
			false
		);
	}
}

#pragma endregion

#pragma region GETTERS AND SETTERS

void UHealthComponent::SetMaxHealth(const int NewMaxHealth)
{
	MaxHealth = NewMaxHealth;

	// If our MaxHealth is smaller than our current health, we need to clamp our current health.
	if (MaxHealth < CurrentHealth)
	{
		SetHealth(MaxHealth);
	}
}

#pragma endregion
