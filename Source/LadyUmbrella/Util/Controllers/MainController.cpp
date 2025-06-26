// 
// MainController.cpp
// 
// Implementation of the MainController class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "MainController.h"
#include "LadyUmbrella/Util/HUDs/MainHUD.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Util/Logger.h"
#include "LadyUmbrella/Util/Localization/LanguageCode.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"

AMainController::AMainController()
{
	// Setting up default Values.
	ActiveControllerInputMappingContextIndex = 0;
	ActiveKeyboardInputMappingContextIndex = 0;
	NoclipSpeedMultiplier = 1.0f;
	LanguageCode = ELanguageCode::EN_US;

	// Rest of Constructor.
	ConstructorHelpers::FObjectFinder<UInputAction> ActivateNextControllerMappingContextActionAsset(TEXT("/Game/Input/InputActions/IA_ActivateNextControllerMappingContext.IA_ActivateNextControllerMappingContext"));
	if (ActivateNextControllerMappingContextActionAsset.Succeeded())
	{
		ActivateNextControllerMappingContextAction = ActivateNextControllerMappingContextActionAsset.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> ActivatePreviousControllerMappingContextActionAsset(TEXT("/Game/Input/InputActions/IA_ActivatePreviousControllerMappingContext.IA_ActivatePreviousControllerMappingContext"));
	if (ActivatePreviousControllerMappingContextActionAsset.Succeeded())
	{
		ActivatePreviousControllerMappingContextAction = ActivatePreviousControllerMappingContextActionAsset.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> ActivateNextKeyboardMappingContextActionAsset(TEXT("/Game/Input/InputActions/IA_ActivateNextKeyboardMappingContext.IA_ActivateNextKeyboardMappingContext"));
	if (ActivateNextKeyboardMappingContextActionAsset.Succeeded())
	{
		ActivateNextKeyboardMappingContextAction = ActivateNextKeyboardMappingContextActionAsset.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> ActivatePreviousKeyboardMappingContextActionAsset(TEXT("/Game/Input/InputActions/IA_ActivatePreviousKeyboardMappingContext.IA_ActivatePreviousKeyboardMappingContext"));
	if (ActivatePreviousKeyboardMappingContextActionAsset.Succeeded())
	{
		ActivatePreviousKeyboardMappingContextAction = ActivatePreviousKeyboardMappingContextActionAsset.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> CycleUmbrellaBulletActionAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_ChangeProjectile.IA_ChangeProjectile'"));
	if (CycleUmbrellaBulletActionAsset.Succeeded())
	{
		CycleUmbrellaBulletAction = CycleUmbrellaBulletActionAsset.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> MoveBetweenCoversActionAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_MoveBetweenCovers.IA_MoveBetweenCovers'"));
	if (MoveBetweenCoversActionAsset.Succeeded())
	{
		MoveBetweenCoversAction = MoveBetweenCoversActionAsset.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> GrappleActionAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Grapple.IA_Grapple'"));
	if (GrappleActionAsset.Succeeded())
	{
		GrappleAction = GrappleActionAsset.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> InteractingActionAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Interact.IA_Interact'"));
	if (InteractingActionAsset.Succeeded())
	{
		InteractingAction = InteractingActionAsset.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> JumpActionAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Jump.IA_Jump'"));
	if (JumpActionAsset.Succeeded())
	{
		JumpAction = JumpActionAsset.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> LookActionAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Look.IA_Look'"));
	if (LookActionAsset.Succeeded())
	{
		LookAction = LookActionAsset.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> MoveActionAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Move.IA_Move'"));
	if (MoveActionAsset.Succeeded())
	{
		MoveAction = MoveActionAsset.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> ReloadActionAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Reload.IA_Reload'"));
	if (ReloadActionAsset.Succeeded())
	{
		ReloadAction =  ReloadActionAsset.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> ShootUmbrellaActionAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Attack.IA_Attack'"));
	if (ShootUmbrellaActionAsset.Succeeded())
	{
		ShootUmbrellaAction = ShootUmbrellaActionAsset.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> ToggleShieldActionAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_ToggleUmbrella.IA_ToggleUmbrella'"));
	if (ToggleShieldActionAsset.Succeeded())
	{
		ToggleShieldAction = ToggleShieldActionAsset.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> HoldShieldActionAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_HoldUmbrella.IA_HoldUmbrella'"));
	if (HoldShieldActionAsset.Succeeded())
	{
		HoldShieldAction = HoldShieldActionAsset.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> CoverActionAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Cover.IA_Cover'"));
	if (CoverActionAsset.Succeeded())
	{
		CoverAction = CoverActionAsset.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> AimActionAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Aim.IA_Aim'"));
	if (AimActionAsset.Succeeded())
	{
		AimAction = AimActionAsset.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> SwapHookActionAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_SwapToHook.IA_SwapToHook'"));
	if (SwapHookActionAsset.Succeeded())
	{
		SwapHookAction = SwapHookActionAsset.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> SwapElectricActionAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_SwapToElectric.IA_SwapToElectric'"));
	if (SwapElectricActionAsset.Succeeded())
	{
		SwapElectricAction = SwapElectricActionAsset.Object;
	}
}

void AMainController::BeginPlay()
{
	Super::BeginPlay();

	// Initializing Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (ActiveControllerInputMappingContextIndex < ControllerInputMappingContexts.Num())
		{
			Subsystem->AddMappingContext(ControllerInputMappingContexts[ActiveControllerInputMappingContextIndex], 0);
		}
		if (ActiveKeyboardInputMappingContextIndex < KeyboardInputMappingContexts.Num())
		{
			Subsystem->AddMappingContext(KeyboardInputMappingContexts[ActiveKeyboardInputMappingContextIndex], 0);
		}
	}
	
	// Setting the HUD
	SetMainHUD(Cast<AMainHUD>(GetHUD()));
}

void AMainController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInput->BindAction(GetPauseInputAction(), ETriggerEvent::Triggered, this, &AMainController::HandleTogglePauseMenu);
		EnhancedInput->BindAction(CheatInputAction, ETriggerEvent::Triggered, this, &AMainController::HandleToggleCheatMenu);
		EnhancedInput->BindAction(CheatInputAction, ETriggerEvent::Completed, this, &AMainController::EnableToggleCheatMenu);
	}
}

void AMainController::SetupPlayerInput(APlayerCharacter* _PlayerCharacter)
{
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Input to Switch Control Schemes
		EnhancedInput->BindAction(ActivateNextControllerMappingContextAction, ETriggerEvent::Started, this, &AMainController::ActivateNextAvailibleControllerInputMappingContext);
		EnhancedInput->BindAction(ActivatePreviousControllerMappingContextAction, ETriggerEvent::Started, this, &AMainController::ActivatePreviousAvailibleControllerInputMappingContext);
		EnhancedInput->BindAction(ActivateNextKeyboardMappingContextAction, ETriggerEvent::Started, this, &AMainController::ActivateNextAvailibleKeyboardInputMappingContext);
		EnhancedInput->BindAction(ActivatePreviousKeyboardMappingContextAction, ETriggerEvent::Started, this, &AMainController::ActivatePreviousAvailibleKeyboardInputMappingContext);
		
		// Basic Actions	
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, _PlayerCharacter, &APlayerCharacter::Jump);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, _PlayerCharacter, &APlayerCharacter::StopJumping);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, _PlayerCharacter, &APlayerCharacter::Move);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Completed, _PlayerCharacter, &APlayerCharacter::StopMovingPlayer);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, _PlayerCharacter, &APlayerCharacter::Look);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Completed, _PlayerCharacter, &APlayerCharacter::StopLook);
		/* Custom Input*/
		// Custom input for Aiming the umbrella
		EnhancedInput->BindAction(AimAction, ETriggerEvent::Started, _PlayerCharacter, &APlayerCharacter::StartedHoldingAimButton);
		// Custom input both for Stop Aiming the umbrella
		EnhancedInput->BindAction(AimAction, ETriggerEvent::Canceled, _PlayerCharacter, &APlayerCharacter::ReleasedAimButton);
		EnhancedInput->BindAction(AimAction, ETriggerEvent::Completed, _PlayerCharacter, &APlayerCharacter::ReleasedAimButton);

		EnhancedInput->BindAction(ReloadAction, ETriggerEvent::Started, _PlayerCharacter, &APlayerCharacter::Reload);
		
		// Custom input for interacting with overlapping elements
		EnhancedInput->BindAction(InteractingAction, ETriggerEvent::Started, _PlayerCharacter, &APlayerCharacter::InteractingWithOverlappingElements);
		// Custom input for Opening and closing the umbrella
		EnhancedInput->BindAction(ToggleShieldAction, ETriggerEvent::Started, _PlayerCharacter, &APlayerCharacter::ToggleShield);

		// Custom input for Opening and Aiming on the Umbrella
		EnhancedInput->BindAction(HoldShieldAction, ETriggerEvent::Started, _PlayerCharacter, &APlayerCharacter::HoldShield);
		EnhancedInput->BindAction(HoldShieldAction, ETriggerEvent::Canceled, _PlayerCharacter, &APlayerCharacter::StopShield);
		EnhancedInput->BindAction(HoldShieldAction, ETriggerEvent::Completed, _PlayerCharacter, &APlayerCharacter::StopShield);
		
		// Custom input for Shooting the Umbrella as a Gun with the Ammo Equipped
		EnhancedInput->BindAction(ShootUmbrellaAction, ETriggerEvent::Started, _PlayerCharacter, &APlayerCharacter::ShootingUmbrella);
		// Custom input for Shooting the Umbrella as a Grapple and Interact with Hit element
		EnhancedInput->BindAction(GrappleAction, ETriggerEvent::Started, _PlayerCharacter, &APlayerCharacter::GadgetShoot);

		EnhancedInput->BindAction(CoverAction, ETriggerEvent::Triggered, _PlayerCharacter, &APlayerCharacter::ToggleCover);
		EnhancedInput->BindAction(MoveBetweenCoversAction, ETriggerEvent::Triggered, _PlayerCharacter, &APlayerCharacter::TryToMoveBetweenCovers);
		
		EnhancedInput->BindAction(CycleUmbrellaBulletAction, ETriggerEvent::Started, _PlayerCharacter, &APlayerCharacter::CycleUmbrellaBullets);
		
		// Swap between 
		EnhancedInput->BindAction(SwapHookAction, ETriggerEvent::Started, _PlayerCharacter, &APlayerCharacter::SwapToHook);
		EnhancedInput->BindAction(SwapElectricAction, ETriggerEvent::Started, _PlayerCharacter, &APlayerCharacter::SwapToElectric);

	}
}

void AMainController::HandleTogglePauseMenu()
{
	if (IsValid(GetHUD()))
	{
		GetMainHUD()->UnTogglePlayerHUD();
		GetMainHUD()->TogglePauseMenu();
	}
}

void AMainController::HandleToggleCheatMenu()
{
	if (IsValid(GetHUD()))
	{
		GetMainHUD()->ToggleCheatMenu();
	}
}

void AMainController::EnableToggleCheatMenu()
{
	if (IsValid(GetHUD()))
	{
		GetMainHUD()->EnableToggleCheatMenu();
	}
}

void AMainController::HandleToggleSettingsMenu() const
{
	if (IsValid(GetHUD()))
	{
		GetMainHUD()->ToggleSettingsMenu();
	}
}

void AMainController::HandleToggleControlSettingsMenu() const
{
	if (IsValid(GetHUD()))
	{
		GetMainHUD()->ToggleControlSettingsMenu();
	}
}

void AMainController::HandleTogglePlayerHUD() const
{
	if (IsValid(GetHUD()))
	{
		GetMainHUD()->TogglePlayerHUD();
	}
}

void AMainController::HandleUnTogglePlayerHUD() const
{
	if (IsValid(GetHUD()))
	{
		GetMainHUD()->UnTogglePlayerHUD();
	}
}

void AMainController::HandleUmbrellaSpawned() const
{
	if (IsValid(GetHUD()))
	{
		GetMainHUD()->OnUmbrellaSpawned(Cast<APlayerCharacter>(GetPawn()));
	}
}

void AMainController::HandleClearWidgets() const
{
	if (IsValid(GetHUD()))
	{
		GetMainHUD()->ClearWidgets();
	}
}

AMainHUD* AMainController::GetMainHUD() const
{
	return MainHUD;
}

UInputAction* AMainController::GetPauseInputAction() const
{
	return PauseInputAction;
}

UInputAction* AMainController::GetCheatInputAction() const
{
	return CheatInputAction;
}

void AMainController::SetMainHUD(AMainHUD* NewMainHUD)
{
	MainHUD = NewMainHUD;
}

#pragma region INPUT FUNCTIONS

UInputMappingContext* AMainController::GetControllerMappingContextAtIndex(int _InputMappingContextIndex) const
{
	// If we tried to get an invalid Input Mapping Context.
	if (_InputMappingContextIndex >= ControllerInputMappingContexts.Num())
	{
#if UE_EDITOR
		FLogger::ErrorLog("Trying to Get Controller Input Mapping Nº%i, but the MainController only has %i availible Controller Mapping Contexts.", _InputMappingContextIndex, ControllerInputMappingContexts.Num());
#endif
		
		return nullptr;
	}

	return ControllerInputMappingContexts[_InputMappingContextIndex];
}

UInputMappingContext* AMainController::GetKeyboardMappingContextAtIndex(int _InputMappingContextIndex) const
{
	// If we tried to get an invalid Input Mapping Context.
	if (_InputMappingContextIndex >= KeyboardInputMappingContexts.Num())
	{
#if UE_EDITOR
		FLogger::ErrorLog("Trying to Get Keyboard Input Mapping Nº%i, but the MainController only has %i availible Keyboard Mapping Contexts.", _InputMappingContextIndex, KeyboardInputMappingContexts.Num());
#endif
		
		return nullptr;
	}

	return KeyboardInputMappingContexts[_InputMappingContextIndex];
}

void AMainController::SetActiveControllerInputMappingContext(int _NewInputMappingContextIndex)
{
	// If we are trying to activate the Mapping Context that is already on.
	if (ActiveControllerInputMappingContextIndex == _NewInputMappingContextIndex)
	{
		return;
	}
	// If we tried to activate an invalid Input Mapping Context.
	if (_NewInputMappingContextIndex >= ControllerInputMappingContexts.Num())
	{
#if UE_EDITOR
		FLogger::ErrorLog("Trying to activate Controller Input Mapping N.%i, but the MainController only has %i availible Controller Mapping Contexts.", _NewInputMappingContextIndex, ControllerInputMappingContexts.Num());
#endif
		
		return;
	}
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->RemoveMappingContext(ControllerInputMappingContexts[ActiveControllerInputMappingContextIndex]);
		ActiveControllerInputMappingContextIndex = _NewInputMappingContextIndex;
		Subsystem->AddMappingContext(ControllerInputMappingContexts[ActiveControllerInputMappingContextIndex], 0);

#if UE_EDITOR
		FString message = FString("Activating Controller Input Mapping N.") + FString::FromInt(ActiveControllerInputMappingContextIndex) + FString(", ") + ControllerInputMappingContexts[ActiveControllerInputMappingContextIndex]->GetName();
		FLogger::WarningLog(message);
#endif
	}
}

void AMainController::SetActiveKeyboardInputMappingContext(int _NewInputMappingContextIndex)
{
	// If we are trying to activate the Mapping Context that is already on.
	if (ActiveKeyboardInputMappingContextIndex == _NewInputMappingContextIndex)
	{
		return;
	}
	// If we tried to activate an invalid Input Mapping Context.
	if (_NewInputMappingContextIndex >= KeyboardInputMappingContexts.Num())
	{
#if UE_EDITOR
		FLogger::ErrorLog("Trying to activate Keyboard Input Mapping N.%i, but the MainController only has %i availible Keyboard Mapping Contexts.", _NewInputMappingContextIndex, KeyboardInputMappingContexts.Num());
#endif
		
		return;
	}
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->RemoveMappingContext(KeyboardInputMappingContexts[ActiveKeyboardInputMappingContextIndex]);
		ActiveKeyboardInputMappingContextIndex = _NewInputMappingContextIndex;
		Subsystem->AddMappingContext(KeyboardInputMappingContexts[ActiveKeyboardInputMappingContextIndex], 0);

#if UE_EDITOR
		FString message = FString("Activating Keyboard Input Mapping N.") + FString::FromInt(ActiveKeyboardInputMappingContextIndex) + FString(", ") + KeyboardInputMappingContexts[ActiveKeyboardInputMappingContextIndex]->GetName();
		FLogger::WarningLog(message);
#endif
	}
}

void AMainController::ActivateNextAvailibleControllerInputMappingContext()
{
	if (ActiveControllerInputMappingContextIndex == ControllerInputMappingContexts.Num() - 1)
	{
		SetActiveControllerInputMappingContext(0);
	}
	else
	{
		SetActiveControllerInputMappingContext(ActiveControllerInputMappingContextIndex + 1);
	}
}

void AMainController::ActivatePreviousAvailibleControllerInputMappingContext()
{
	if (ActiveControllerInputMappingContextIndex == 0)
	{
		SetActiveControllerInputMappingContext(ControllerInputMappingContexts.Num() - 1);
	}
	else
	{
		SetActiveControllerInputMappingContext(ActiveControllerInputMappingContextIndex - 1);
	}
}

void AMainController::ActivateNextAvailibleKeyboardInputMappingContext()
{
	if (ActiveKeyboardInputMappingContextIndex == KeyboardInputMappingContexts.Num() - 1)
	{
		SetActiveKeyboardInputMappingContext(0);
	}
	else
	{
		SetActiveKeyboardInputMappingContext(ActiveKeyboardInputMappingContextIndex + 1);
	}
}

void AMainController::ActivatePreviousAvailibleKeyboardInputMappingContext()
{
	if (ActiveKeyboardInputMappingContextIndex == 0)
	{
		SetActiveKeyboardInputMappingContext(KeyboardInputMappingContexts.Num() - 1);
	}
	else
	{
		SetActiveKeyboardInputMappingContext(ActiveKeyboardInputMappingContextIndex - 1);
	}
}

#pragma endregion 

/****************************/
/*          CHEATS          */
/****************************/

void AMainController::Godmode()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->SetGodmode(!PlayerCharacter->IsGodmode());
	}
}

void AMainController::NoClip()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->SetNoClip(!PlayerCharacter->IsNoClip());
		
		// NOTE: This doesn't work for some reason? Maybe it's multithreaded?
		// PlayerCharacter->SetActorEnableCollision(!PlayerCharacter->IsNoClip());
		
		if (PlayerCharacter->IsNoClip())
		{
			PlayerCharacter->GetCharacterMovement()->MaxFlySpeed *= NoclipSpeedMultiplier;
			PlayerCharacter->ClientCheatFly();
			PlayerCharacter->SetActorEnableCollision(false);
		}
		else
		{
			PlayerCharacter->GetCharacterMovement()->MaxFlySpeed /= NoclipSpeedMultiplier;
			PlayerCharacter->ClientCheatWalk();
			PlayerCharacter->SetActorEnableCollision(true);
		}
		
		PlayerCharacter->bUseControllerRotationPitch = PlayerCharacter->IsNoClip();
		PlayerCharacter->bUseControllerRotationYaw = PlayerCharacter->IsNoClip();
	}
}

void AMainController::InfiniteAmmo()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (IsValid(Player))
	{
		AUmbrella* Umbrella = PlayerCharacter->GetUmbrella();

		if (IsValid(Umbrella))
		{
			Umbrella->SetInfiniteAmmo(!Umbrella->IsInfiniteAmmo());			
		}
	}
}

void AMainController::ReloadLevel()
{
	const FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());

	UGameplayStatics::OpenLevel(GetWorld(), FName(LevelName));
}

void AMainController::SetAmmoCurrent(const int Amount)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (IsValid(Player))
	{
		AUmbrella* Umbrella = PlayerCharacter->GetUmbrella();

		if (IsValid(Umbrella))
		{
			Umbrella->SetAmmoCurrent(Amount);
		}
	}
}

void AMainController::SetAmmoReserve(const int Amount)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (IsValid(Player))
	{
		AUmbrella* Umbrella = PlayerCharacter->GetUmbrella();

		if (IsValid(Umbrella))
		{
			Umbrella->SetAmmoReserve(Amount);
		}
	}
}

void AMainController::SetComponents(const int Amount)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (IsValid(Player))
	{
		AUmbrella* Umbrella = PlayerCharacter->GetUmbrella();

		if (IsValid(Umbrella))
		{
			Umbrella->SetAmmoReserve(Amount);
		}
	}
}