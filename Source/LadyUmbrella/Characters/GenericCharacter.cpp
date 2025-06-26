// 
// CoreCharacter.cpp
// 
// Implementation of the CoreCharacter class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "GenericCharacter.h"

#include "AIController.h"
#include "HealthComponent.h"
#include "InteractiveMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "../Util/SecondOrderSystems/SecondOrder1DSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Environment/CoverSystem/PlayerCoverMovementComponent.h"
#include "LadyUmbrella/Util/Logger.h"
#include "LadyUmbrella/Util/Controllers/MainController.h"
#include "LadyUmbrella/Util/Localization/LanguageCode.h"
#include "LadyUmbrella/Util/Localization/LocalizedStringRow.h"
#include "Player/PlayerCharacter.h"


AGenericCharacter::AGenericCharacter()
{
	// Initializing Variables
	bIsAiming = false;
	bIsElectrified = false;
	bIsStunned = false;
	ElectrifiedDuration = 5.0f;
	InCoverAimingDirection = INVALID_IN_COVER_AIMING_DIRECTION;
	CharacterName = FString("Character");

	// Rest of Constructor
	PrimaryActorTick.bCanEverTick = true;

	CoverTraversingSpeedSystem = CreateDefaultSubobject<USecondOrder1DSystem>("CoverTraversingSpeedSystem");
	CoverOrientationSystem = CreateDefaultSubobject<USecondOrder1DSystem>("CoverOrientationSystem");

	CoverTraversingSpeedSystem->ChangeConstants(2,1,0.2f);
	CoverOrientationSystem->ChangeConstants(4,1,0.1f);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	InteractiveMovementComponent = CreateDefaultSubobject<UInteractiveMovementComponent>("InteractiveMovementComponent");
	
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ElectricFXObj(TEXT("/Script/Niagara.NiagaraSystem'/Game/StarterContent/Particles/NS_Electric.NS_Electric'"));
	if (ElectricFXObj.Succeeded())
	{
		ElectricFXTemplate = ElectricFXObj.Object;
	}

	MainController = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)); 

	// If this is a Player, we wanna give it the extended CoverMovementComponent
	if (Cast<APlayerCharacter>(this))
	{
		CoverMovementComponent = CreateDefaultSubobject<UPlayerCoverMovementComponent>(TEXT("PlayerCoverMovementComponent"));
	}
	else // Else, you get a regular Character CoverMovementComponent
	{
		CoverMovementComponent = CreateDefaultSubobject<UCharacterCoverMovementComponent>(TEXT("CharacterCoverMovementComponent"));
	}
}

#pragma region VIRTUAL FUNCTIONS

void AGenericCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(HealthComponent))
	{
		HealthComponent->SetMaxHealth(100.0f);
		HealthComponent->SetHealth(100.0f); //to be defined

		// Bind to the OnDeathDelegate
		if (HealthComponent)
		{
			HealthComponent->OnDeathDelegate.AddDynamic(this, &AGenericCharacter::OnDeath);
		}
	}
}

void AGenericCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

#pragma endregion 

#pragma region HEALTH INTERFACE FUNCTIONS IMPLEMENTATION

void AGenericCharacter::SubtractHealth(const float HealthToSubtract,enum EWeaponType DamageType)
{
	if (!IsValid(HealthComponent))
	{
#if UE_EDITOR
		FLogger::ErrorLog("Trying to Substract from Actor's Health but could not find its HealthComponent.");
#endif
		return;	
	}

	// Save last damage type
	LastDamageType = DamageType;
	
	HealthComponent->SubtractHealth(HealthToSubtract);
}

void AGenericCharacter::SetHealth(const float NewHealth)
{
	if (!IsValid(HealthComponent))
	{
#if UE_EDITOR
		FLogger::ErrorLog("Trying to Set Actor's Health but could not find its HealthComponent.");
#endif
		return;	
	}
	
	HealthComponent->SetHealth(NewHealth);
}

const float AGenericCharacter::GetHealth() const
{
	return IsValid(HealthComponent) ? HealthComponent->GetHealth() : 0;
}

#pragma endregion

#pragma region COVER SECTION

bool AGenericCharacter::IsMovementControlledByCover() const
{
	return IsValid(CoverMovementComponent) && CoverMovementComponent->IsMovementControlledByCover();
}

float AGenericCharacter::GetInCoverOrientation() const
{
	if (!IsValid(CoverMovementComponent))
	{
		return 0;
	}

	return CoverMovementComponent->GetInCoverOrientation();
}

float AGenericCharacter::GetInCoverSpeed() const
{
	if (!IsValid(CoverMovementComponent))
	{
		return 0;
	}

	return CoverMovementComponent->GetInCoverSpeed();
}

FVector AGenericCharacter::GetInCoverAimingDirection() const
{
	return InCoverAimingDirection;
}

bool AGenericCharacter::IsInTallCover() const
{
	if (IsValid(CoverMovementComponent))
	{
		return CoverMovementComponent->IsInTallCover();
	}

	return false;
}

bool AGenericCharacter::NotifyBeginTravellingToCover(UCoverSpline* CoverToEnter, float SectionToEnter)
{
	if (IsValid(CoverMovementComponent))
	{
		return CoverMovementComponent->NotifyBeginTravellingToCover(CoverToEnter, SectionToEnter);
	}

	return false;
}

void AGenericCharacter::NotifyReachedCoverTravellingTowards()
{
	if (IsValid(CoverMovementComponent))
	{
		return CoverMovementComponent->NotifyReachedCoverTravellingTowards();
	}
}

void AGenericCharacter::NotifyExitCover()
{
	if (IsValid(CoverMovementComponent))
	{
		return CoverMovementComponent->NotifyExitCover();
	}
}


#pragma endregion

#pragma region COMBAT FUNCTIONS

bool AGenericCharacter::IsAiming() const
{
	return bIsAiming;
}

bool AGenericCharacter::SetIsAiming(const bool IsAiming)
{
	if(bIsAiming != IsAiming)
	{
		bIsAiming = IsAiming;
		if(IsValid(CoverMovementComponent) && CoverMovementComponent->GetCoveredState() == ECoveredState::InCover)
		{
			if(IsAiming == true)
			{
				CoverMovementComponent->SetInsideCoverState(EInsideCoverState::Aiming);
			}
			else
			{
				CoverMovementComponent->SetInsideCoverState(EInsideCoverState::BaseCovered);
			}
		}
	}

	return bIsAiming;
}

bool AGenericCharacter::IsStrafing() const
{
	return bIsStrafing;
}

bool AGenericCharacter::IsInZipline() const
{
	return bInZipline;
}

void AGenericCharacter::OnDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Generic Character On Death"));
	SetDead();
}

void AGenericCharacter::SetDead()
{
	if (IsValid(HealthComponent) && HealthComponent->IsDead())
	{
		// Lanza el delegate
		OnDeathChanged.Broadcast(this);
	}
}

void AGenericCharacter::SetElectrified(bool IsElectrified)
{
	bIsElectrified = IsElectrified;
	bIsStunned = bIsElectrified;
	if (IsElectrified)
	{
		GetCharacterMovement()->DisableMovement();

		//exit cover
		NotifyExitCover();

		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			AIController->StopMovement();
			AIController->SetActorTickEnabled(false);
		}
		if (bIsElectrified && ElectricFXTemplate)
		{
			ActiveElectricFX = UNiagaraFunctionLibrary::SpawnSystemAttached(
				ElectricFXTemplate,
				GetMesh(),
				NAME_None,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget,
				false // do not auto destroy
			);
		}
		GetWorldTimerManager().SetTimer(ElectrifiedResetTimer, this, &AGenericCharacter::ResetElectrified, ElectrifiedDuration, false);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(ElectrifiedResetTimer);
	}
}


void AGenericCharacter::ApplyStun(float Duration)
{
	bIsStunned = true;
	GetWorld()->GetTimerManager().SetTimer(StunTimerHandle, this, &AGenericCharacter::EndStun, Duration, false);
}

void AGenericCharacter::EndStun()
{
	bIsSmoked = false;
	bIsStunned = false;
}

bool AGenericCharacter::PlayMontage(UAnimMontage* Montage, float PlayRate) const
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance) && IsValid(Montage))
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_SetPlayRate(Montage, PlayRate);
		return true;
	}
	return false;
}

void AGenericCharacter::ResetElectrified()
{
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->SetActorTickEnabled(true);
	}
	if (ActiveElectricFX)
	{
		ActiveElectricFX->DestroyComponent();
		ActiveElectricFX = nullptr;
	}
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	bIsElectrified = false;
	bIsStunned = false;
	UE_LOG(LogTemp, Log, TEXT("%s is no longer electrified."), *GetName());
}

void AGenericCharacter::SpeakRandomActionLine(const EActionCode& ActionCode) const
{
	if (!IsValid(LocalizedStrings))
	{
		FLogger::ErrorLog("Failed to find valid line for %d.", ActionCode);
		return;
	}
	
	TArray<FLocalizedStringRow*> AllRows;
	TArray<FLocalizedStringRow*> MatchingRows;
	
	LocalizedStrings->GetAllRows<FLocalizedStringRow>(TEXT("Context"), AllRows);

	for (FLocalizedStringRow* Row : AllRows)
	{
		if (Row && Row->ActionCode == ActionCode)
		{
			MatchingRows.Add(Row);
		}
	}

	if (MatchingRows.IsEmpty())
	{
		FLogger::ErrorLog("Failed to find valid line for %d.", ActionCode);
		return;
	}

	const FString LocalizedString = GetLocalizedString(MatchingRows, FMath::RandRange(0, MatchingRows.Num() - 1));

	// TODO: Play audio file.
	// TODO: Coordinate audio files to avoid overlapping tracks.
	// TODO: Show LocalizedString on screen (if subtitles are enabled).
}

const FString AGenericCharacter::GetLocalizedString(const TArray<FLocalizedStringRow*> Rows, const uint8 Index) const
{
	switch (MainController->GetLanguageCode())
	{
	case ELanguageCode::ES_ES:
		return Rows[Index]->ES_ES;	
	default:
	case ELanguageCode::EN_US:
		return Rows[Index]->EN_US;
	}
}

#pragma endregion
