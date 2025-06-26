// 
// PlayerHealth.cpp
// 
// Implementation of the PlayerHealth class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 


#include "PlayerHealth.h"
#include "Components/TextBlock.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"

void UPlayerHealth::Show()
{
	if (APlayerCharacter* MainCharacter = Cast<APlayerCharacter>(GetOwningPlayer()->GetCharacter()))
	{
		UHealthComponent* HealthComponent = MainCharacter->GetHealthComponent();
		if (IsValid(HealthComponent))
		{
			HealthComponent->OnHealthAmmountChanged.AddDynamic(this, &UPlayerHealth::UpdatePlayerHealthWithDelegate);
		}
	}
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UPlayerHealth::Hide()
{
	if (APlayerCharacter* MainCharacter = Cast<APlayerCharacter>(GetOwningPlayer()->GetCharacter()))
	{
		UHealthComponent* HealthComponent = MainCharacter->GetHealthComponent();
		if (IsValid(HealthComponent))
		{
			HealthComponent->OnHealthAmmountChanged.RemoveDynamic(this, &UPlayerHealth::UpdatePlayerHealthWithDelegate);
		}
	}
	SetVisibility(ESlateVisibility::Hidden);
}

UTextBlock* UPlayerHealth::GetHealthText() const
{
	return HealthText;
}

void UPlayerHealth::SetHealthText(UTextBlock* NewPlayerHealthText)
{
	HealthText = NewPlayerHealthText;
}

void UPlayerHealth::UpdatePlayerHealthWithDelegate(int32 NewHealth, bool bLostHealth)
{
	UpdateProgressBar(static_cast<float>(NewHealth));
}

void UPlayerHealth::UpdateProgressBar(const float Value)
{
	Super::UpdateProgressBar(Value);
	
	if (Value <= 0)
	{
		GetHealthText()->SetText(FText::FromString(FString::Printf(TEXT("%d"), 0)));
	}
	else
	{
		GetHealthText()->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int>(Value))));
	}
}

void UPlayerHealth::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (const APlayerCharacter* MainCharacter = Cast<APlayerCharacter>(GetOwningPlayer()->GetCharacter()))
	{
		SetMaximumValue(MainCharacter->GetHealth());
		UpdateProgressBar(MainCharacter->GetHealth());
	}
}

