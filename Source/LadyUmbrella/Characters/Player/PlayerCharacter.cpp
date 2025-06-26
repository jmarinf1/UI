// 
// PlayerCharacter.cpp
// 
// Implementation of the PlayerCharacter class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "PlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Runtime/CinematicCamera/Public/CineCameraComponent.h"
#include "CableComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "Engine/LocalPlayer.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LadyUmbrella/Characters/HealthComponent.h"
#include "LadyUmbrella/Characters/InteractiveMovementComponent.h"
#include "LadyUmbrella/Interfaces/InteractInterface.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "LadyUmbrella/Environment/CoverSystem/CoverParent.h"
#include "LadyUmbrella/Weapons/GenericProjectile.h"
#include "LadyUmbrella/Util/MathLibraries/EasingLibrary.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "MotionWarpingComponent.h"
#include "LadyUmbrella/Environment/CheckPointTrigger.h"
#include "LadyUmbrella/Environment/CoverSystem/PlayerCoverMovementComponent.h"
#include "LadyUmbrella/Util/Logger.h"
#include "LadyUmbrella/Items/InteractuableObjects/PickableObjects/Ammo.h"
#include "LadyUmbrella/Items/InteractuableObjects/PickableObjects/UpgradePieces.h"
#include "LadyUmbrella/Util/AimAssistComponent.h"
#include "LadyUmbrella/Environment/CoverSystem/FreeRoamCoverComponent.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/Controllers/MainController.h"
#include "LadyUmbrella/UI/HitReactionComponent.h"

APlayerCharacter::APlayerCharacter()
{
	// Initializing variables.
	bIsAimingButtonPressed = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Melee Range
	MeleeRange = 150.0f;

	// Save Game Struct, not needed in the player
	PlayerSaveStruct.Health = 100;

	CameraLookMultiplierAimAssist = 1.0f;
	MaxWalkSpeed				= 500.0f;
	RecoilAnimSpeedOffset		= 0.5f;
	bResetOnCover				= false;
	bIsManuallyAiming			= false;

	WeaponType				 = EWeaponType::Umbrella;
	CurrentPiecesForUpgrades = 0;
	StrafeWalkSpeed			 = 300.f;

	CharacterName = FString("Francesca");
	const FString& LocalizedStringPath = "/Script/Engine.DataTable'/Game/DataStructures/DataTables/DT_LocalizedStrings_Francesca.DT_LocalizedStrings_Francesca'";
	LocalizedStrings = AssetUtils::FindObject<UDataTable>(LocalizedStringPath);
	
	// Rest of the Constructor.
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::GetHit);
		GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnEndOverlap);
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;	
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
		GetCharacterMovement()->JumpZVelocity = 700.f;
		GetCharacterMovement()->AirControl = 0.35f;
		GetCharacterMovement()->MaxWalkSpeed = 500.f;
		GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
		GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
		GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	}
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom")); // Moving
	if (IsValid(CameraBoom))
	{
		CameraBoom->SetupAttachment(RootComponent);
		CameraBoom->TargetArmLength = 400.f;
		CameraBoom->bUsePawnControlRotation = true;
		CameraBoom->bEnableCameraLag = true;
		CameraBoom->bDoCollisionTest = false;
		CameraBoom->bInheritRoll	 = false;
	}

	FollowCamera = CreateDefaultSubobject<UCineCameraComponent>(TEXT("FollowCamera")); // Moving
	if (IsValid(FollowCamera))
	{
		FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
		FollowCamera->bUsePawnControlRotation = false;
		FollowCamera->FocusSettings.FocusMethod = ECameraFocusMethod::Tracking;
	}

	CameraSystem = CreateDefaultSubobject<UCustomCameraSystemComponent>(TEXT("CustomCameraSystem"));

	// Initialize Melee TimeLine
	MeleeTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("Melee Timeline"));

	// AI
	FleeRadius = 1500.f;
    CloseRadius = 400.f;
	
#pragma region ASSETS_FINDERS

	ConstructorHelpers::FObjectFinder<UAnimMontage> MeleeMontageAsset(TEXT("/Script/Engine.AnimMontage'/Game/Animations/Montages/AllCharacters/MeleeCombat/AM_ThrustRM.AM_ThrustRM'"));
	if (MeleeMontageAsset.Succeeded())
	{
		MeleeMontage = MeleeMontageAsset.Object;
	}

	ConstructorHelpers::FObjectFinder<UAnimMontage> GrappleMontageAsset(TEXT("/Script/Engine.AnimMontage'/Game/Animations/Montages/AllCharacters/RangeCombat/AM_Grapple.AM_Grapple'"));
	if (GrappleMontageAsset.Succeeded())
	{
		GrapplePullMontage = GrappleMontageAsset.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UAnimMontage> RecoilMontageAsset(TEXT("/Script/Engine.AnimMontage'/Game/Animations/Montages/AllCharacters/RangeCombat/AM_Recoil.AM_Recoil'"));
	if (RecoilMontageAsset.Succeeded())
	{
		RecoilMontage = RecoilMontageAsset.Object;
	}

	ConstructorHelpers::FObjectFinder<UCurveFloat> MeleeRotationCurveAsset(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Combat/C_MeleeRotation.C_MeleeRotation'"));
	if (MeleeRotationCurveAsset.Succeeded())
	{
		MeleeMovementCurve = MeleeRotationCurveAsset.Object;
	}

	ConstructorHelpers::FClassFinder<AUmbrella> UmbrellaBlueprintClass(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Attachments/Interactive/Weapons/BP_Umbrella.BP_Umbrella_C'"));
	if (UmbrellaBlueprintClass.Succeeded())
	{
		UmbrellaSubclass = UmbrellaBlueprintClass.Class;
	}

	RowName = "CH_Francesca";
	ConstructorHelpers::FObjectFinder<UDataTable> DataTableAsset(TEXT("/Script/Engine.DataTable'/Game/DataStructures/DataTables/DT_LadyUmbrellaSystems_FrancescaData.DT_LadyUmbrellaSystems_FrancescaData'"));
	if (DataTableAsset.Succeeded())
	{
		// maybe DataTablePtr is not really needed, but just in case we want it exposed to the designers.
		DataTablePtr			   = DataTableAsset.Object;
		DataTableRowHandler.DataTable = DataTableAsset.Object;
		DataTableRowHandler.RowName	  = RowName;
	}

#pragma endregion

	// Delete after we stop using the temp character, done for proportions and metrics
	if (USkeletalMeshComponent* Comp = FindComponentByClass<USkeletalMeshComponent>())
	{
		ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/Meshes/SkeletalMeshes/SK_BasicMesh.SK_BasicMesh'"));
		if (SkeletalMeshAsset.Succeeded())
		{
			Comp->SetSkeletalMesh(SkeletalMeshAsset.Object);
			Comp->SetRelativeScale3D(FVector(0.75f,0.75f,0.93f));
		}
	}
	
	// MotionWarping
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));

	// Aim Assist
	AimAssistComponent = CreateDefaultSubobject<UAimAssistComponent>(TEXT("AimAssistComponent"));

	// Free Roam Covers
	FreeRoamCoverComponent = CreateDefaultSubobject<UFreeRoamCoverComponent>(TEXT("FreeRoamCoverComponent"));
	
	// UI Hit React Component
	HitReactionComponent = CreateDefaultSubobject<UHitReactionComponent>(TEXT("HitReactionComponent"));


	if (HealthComponent != nullptr)
	{
		HealthComponent->SetCanRegenerate(true);
	}

	// Player State Component
	PlayerStateComponent = CreateDefaultSubobject<UPlayerStateComponent>(TEXT("PlayerStateComponent"));

	// The component that smoothes the camera when in cover.
	InCoverCameraHorizontalAdditionalRatioSmooth = CreateDefaultSubobject<USecondOrder1DSystem>(TEXT("InCoverCameraHorizontalRatioSmooth"));
	if (InCoverCameraHorizontalAdditionalRatioSmooth != nullptr)
	{
		InCoverCameraHorizontalAdditionalRatioSmooth->ChangeConstants(1.5f, 0.9f, 0.5f);
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(Controller))
	{
		PlayerSaveStruct.WorldTransform = Controller->GetLevelTransform();
	}
	float Radius = 0.0f, Height = 0.0f;
	GetCapsuleComponent()->GetScaledCapsuleSize(Radius, Height);

	// Camera 
	if (IsValid(CameraSystem))
	{
		CameraSystem->SetUpCamera(CameraBoom, FollowCamera);
	}
	if (IsValid(CameraBoom))
	{
		CameraBoom->SetRelativeLocation(FVector(0,0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.75f));
	}

	Umbrella = GetWorld()->SpawnActor<AUmbrella>(UmbrellaSubclass);
	if (IsValid(Umbrella))
	{
		if (USkeletalMeshComponent* Comp = FindComponentByClass<USkeletalMeshComponent>())
		{
			Umbrella->AttachToComponent(
				Comp,
				FAttachmentTransformRules(
					EAttachmentRule::SnapToTarget,
					EAttachmentRule::SnapToTarget,
					EAttachmentRule::SnapToTarget,
					true),
				"WeaponSocket"
				);
		}

		//need to create the hud widget after the umbrella is created or the delegate binding doesn't work for the ammo
		if (IsValid(MainController.Get()))
		{
			MainController->HandleUmbrellaSpawned();
		}

		(void) Umbrella->OnCurrentBulletsChanged.ExecuteIfBound(Umbrella->GetAmmoCurrent());
		(void) Umbrella->OnTotalBulletsChanged.ExecuteIfBound(Umbrella->GetAmmoReserve());
	}
	
	if (IsValid(MeleeTimeLine))
	{
		// Melee Timeline to rotate character
		FOnTimelineFloat MeleeProgressUpdate;
		MeleeProgressUpdate.BindUFunction(this, FName("MeleeTimelineProgress"));
		if (IsValid(MeleeMovementCurve))
		{
			MeleeTimeLine->AddInterpFloat(MeleeMovementCurve, MeleeProgressUpdate);
		}
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(CameraSystem))
	{
		CameraSystem->CameraTick(DeltaTime);
	}
	
#if UE_EDITOR
	// IA debug Radius
	// FVector Begin = GetActorLocation();
	// FVector End = FVector(GetActorLocation().X,GetActorLocation().Y,GetActorLocation().Z + 100.f)
	// DrawDebugCylinder(GetWorld(),Begin,End, CloseRadius, 20, FColor::Red, false);
	// DrawDebugCylinder(GetWorld(),Begin,End, FleeRadius, 20, FColor::Yellow, false);

	// Debugging PlayerState
	// FLogger::DebugLog(FString("PlayerState = ") + ToString(GetPlayerGeneralState()));
#endif
	
	// Aiming pitch calculation for animations
	if (IsAiming())
	{
		AimOffsetPitchVal = GetBaseAimRotation().Pitch;
	}

	// Interactive Movement
	if (GetCharacterMovement()->IsFalling())
	{
		// Changing the state to falling if we were not doing it yet.
		SetPlayerGeneralState(EPlayerState::Falling);
		CheckMantle();		
	}
	else if (GetPlayerGeneralState() == EPlayerState::Falling) // If we are no longer falling but our PlayerState is Falling
	{
		SetPlayerGeneralState(EPlayerState::FreeMovement);
	}
}

void APlayerCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (IsValid(MainController.Get()))
	{
		MainController->SetupPlayerInput(this);
	}
}

#pragma region DEATH AND RESPAWN

void APlayerCharacter::KillPlayer() 
{
	if (IsValid(HealthComponent))
	{
		HealthComponent->SubtractHealth(HealthComponent->GetHealth());
	}
}

void APlayerCharacter::SetDead()
{
	Super::SetDead();
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance))
	{
		AnimInstance->StopAllMontages(0.f);
		Controller->SetIgnoreMoveInput(false);
	}
	
	GetMesh()->SetVisibility(false);

	// Disappear and reload the umbrella.
	if (IsValid(Umbrella))
	{
		Umbrella->AutoReload();
		Umbrella->SetVisibility(false);
	}
	
	// If im covered, EXIT
	if (IsValid(CoverMovementComponent) && CoverMovementComponent->IsMovementControlledByCover() || CoverMovementComponent->IsTravellingToCover()) 
	{ 
		NotifyExitCover(); 
	}
	
	// If im aiming, STOP
	if (IsAiming()) 
	{
		ResetAiming(); 
	}
	
	// Reset camera to original state
	CameraSystem->ResetCameraPosition();
	CameraSystem->ResetCameraParams();
	
	// Disable input
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		PlayerController->DisableInput(PlayerController);
	}
	SetPlayerGeneralState(EPlayerState::ControlsDisabled);
	
	//Init Timer to Respawn
	GetWorld()->GetTimerManager().SetTimer
	 (
		RespawnTimerHandle,
		this,
		&APlayerCharacter::Respawn,
		5,
		true,
		3
	);
}

void APlayerCharacter::Respawn()
{
	Umbrella->SetVisibility(true);
	
	GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
	CameraSystem->ResetCameraPosition();

	// Move to Checkpoint
	ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance());
	if (IsValid(GameInstance))
	{
		GameInstance->LoadCheckpoint();
	}
	
	// Enable input
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		PlayerController->EnableInput(PlayerController);
	}
	SetPlayerGeneralState(EPlayerState::FreeMovement);
	
	// Making Mesh visible
	GetMesh()->SetVisibility(true);
	
	// Reset life
	HealthComponent->SetHealth(HealthComponent->GetMaxHealth());
}

#pragma endregion 

#pragma region COVER SYSTEM

void APlayerCharacter::ToggleCover()
{
	SetPlayerDevice();
	
	UPlayerCoverMovementComponent* PlayerCoverMovementComponent = Cast<UPlayerCoverMovementComponent>(CoverMovementComponent);
	if (!IsValid(PlayerCoverMovementComponent))
	{
		return;
	}
	
	// If we were in a cover, OR travelling towards a cover, we Exit cover, which returns the character to a normal state.
	if (CoverMovementComponent->IsMovementControlledByCover() || CoverMovementComponent->IsTravellingToCover())
	{
		if (IsValid(CameraBoom))
		{
			CameraBoom->SetRelativeLocation(FVector(0,0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.75f));
		}

		NotifyExitCover();
	}
	// Else we check if we can enter a Cover.
	else if (CanChangeToPlayerState(EPlayerState::InCover))
	{
		if (PlayerCoverMovementComponent->PlayerTryToEnterCover())
		{
			// Entering a cover closes the shield.
			if (IsValid(Umbrella) && Umbrella->IsOpen())
			{
				Umbrella->Close();
			}
			
			// Just in case, we reset the other system if they were both used at the same time.
			if (FreeRoamCoverComponent && IsInFreeRoamCover)
			{
				FreeRoamCoverComponent->ResetCoverVariables();
			}

			if (IsValid(CameraBoom))
			{
				CameraBoom->SetRelativeLocation(FVector(0,0, GetMesh()->GetLocalBounds().GetBox().GetSize().Z / 2.5f));
			}
		}
		else
		{
			if (FreeRoamCoverComponent) FreeRoamCoverComponent->StartCover();
		}
	}
}

void APlayerCharacter::TryToMoveBetweenCovers()
{
	SetPlayerDevice();

	UPlayerCoverMovementComponent* PlayerCoverMovementComponent = Cast<UPlayerCoverMovementComponent>(CoverMovementComponent);
	if (IsValid(PlayerCoverMovementComponent) && GetPlayerGeneralState() == EPlayerState::InCover)
	{
		PlayerCoverMovementComponent->TryToMoveBetweenCovers();
	}
}

void APlayerCharacter::TurnCoverUIOn()
{
	// FLogger::DebugLog("Toggle CoverUI ON");
}

void APlayerCharacter::TurnCoverUIOff()
{
	// FLogger::DebugLog("Toggle CoverUI OFF");
}

#pragma endregion

#pragma region HEALTH INTERFACE

void APlayerCharacter::SubtractHealth(const float HealthToSubtract, EWeaponType DamageType, FVector LastDamageSourceLocation)
{
	if (IsGodmode())
	{
		return;
	}
	if (HitReactionComponent)
	{
		HitReactionComponent->ShowHitMarker(LastDamageSourceLocation);
	}

	Super::SubtractHealth(HealthToSubtract,DamageType);
}

#pragma endregion

#pragma region THIRD PERSON SHOOTING

void APlayerCharacter::StartedHoldingAimButton()
{
	bIsAimingButtonPressed = true;

	StartAiming();
}

void APlayerCharacter::ReleasedAimButton()
{
	bIsAimingButtonPressed = false;

	ResetAiming();
}

void APlayerCharacter::StartAiming()
{
	SetPlayerDevice();

	// If we cannot enter aiming mode, we don't continue.
	if (!(GetPlayerGeneralState() == EPlayerState::InCover && CanChangeToPlayerState(EPlayerState::InCoverAiming))
		&& !CanChangeToPlayerState(EPlayerState::AimingUmbrella))
	{
		return;
	}

	// If we are travelling to cover (moving between covers or travelling to one), we cannot try to aim.
	if (IsValid(CoverMovementComponent) && CoverMovementComponent->IsTravellingToCover())
	{
		return;
	}
	
	CameraSystem->SetCameraState(FCameraStateKey::AIM);
	SetIsAiming(true);
	SetIsStrafing(true);
	bIsManuallyAiming = true;

	if (UCharacterMovementComponent* UCharacterMovementComponent = GetCharacterMovement())
	{
		UCharacterMovementComponent->bOrientRotationToMovement = false;
		UCharacterMovementComponent->MaxWalkSpeed = StrafeWalkSpeed;

		// If we are aiming in cover.
		if (IsValid(CoverMovementComponent) && CoverMovementComponent->IsMovementControlledByCover())
		{
			SetPlayerGeneralState(EPlayerState::InCoverAiming);
		}
		// If we are aiming normally.
		else
		{
			UCharacterMovementComponent->bUseControllerDesiredRotation = true;
			SetPlayerGeneralState(EPlayerState::AimingUmbrella);
		}
	}

	if (AimAssistComponent)
	{
		AimAssistComponent->OnStartAiming();
	}
}

void APlayerCharacter::ResetAiming()
{
	// We do not need to Reset Aiming if we are not aiming. This can happen when we cancel an Aim not via input.
	if (!IsAiming())
	{
		return;
	}
	
	bIsManuallyAiming = false;
	const bool bHoldShieldActionHeld = IsValid(Umbrella) ? Umbrella->IsOpen() : false;
	AMainController* PC = Cast<AMainController>(GetWorld()->GetFirstPlayerController());
	
	if (bHoldShieldActionHeld && GetPlayerDevice() != "KBM" && PC->GetControllerInputMappingContextIndex() == 1)
	{
		return;
	}
	
	SetIsAiming(false);
	
	if (IsValid(Umbrella))
	{
		Umbrella->Close();

		if (UWidgetComponent* EnergyBar = Umbrella->GetEnergyBarComponent())
		{
			EnergyBar->SetVisibility(false);
		}
	}

	// If we are stopping our aim while in cover.
	if (IsMovementControlledByCover())
	{
		CameraSystem->SetCameraState(FCameraStateKey::COVER);
		SetPlayerGeneralState(EPlayerState::InCover);
	}
	// If we are stopping our aim in regular time.
	else
	{
		CameraSystem->SetCameraState(FCameraStateKey::DEFAULT);
		SetPlayerGeneralState(EPlayerState::FreeMovement);
	}
	
	UCharacterMovementComponent* UCharacterMovementComponent = GetCharacterMovement();
	if (IsValid(UCharacterMovementComponent))
	{
		UCharacterMovementComponent->MaxWalkSpeed = MaxWalkSpeed;
		UCharacterMovementComponent->bUseControllerDesiredRotation = false;
	}
	if (!bIsInCombatVolume)
	{
		SetIsStrafing(false);
		
		if (IsValid(UCharacterMovementComponent))
		{
			UCharacterMovementComponent->bOrientRotationToMovement = true;
			UCharacterMovementComponent->bUseControllerDesiredRotation = false;
		}
	}

	if (AimAssistComponent)
	{
		AimAssistComponent->StopRotation();
		CameraSystem->ResetCameraPosition();
	}
}

#pragma endregion

#pragma region PLAYER STATE

UPlayerStateComponent* APlayerCharacter::GetPlayerStateComponent() const
{
	return PlayerStateComponent;
}

EPlayerState APlayerCharacter::GetPlayerGeneralState() const
{
	if (IsValid(GetPlayerStateComponent()))
	{
		return GetPlayerStateComponent()->GetPlayerState();
	}

#if UE_EDITOR
	FLogger::ErrorLog("Couldn't Find Player State Component.");
#endif
	
	return EPlayerState::FreeMovement;
}

bool APlayerCharacter::SetPlayerGeneralState(EPlayerState NewPlayerState)
{
	if (IsValid(GetPlayerStateComponent()))
	{
		return GetPlayerStateComponent()->SetPlayerState(NewPlayerState);
	}

#if UE_EDITOR
	FLogger::ErrorLog("Couldn't Find Player State Component.");
#endif

	return false;
}

bool APlayerCharacter::CanChangeToPlayerState(EPlayerState NewPlayerState)
{
	if (IsValid(GetPlayerStateComponent()))
	{
		return GetPlayerStateComponent()->CanChangeToPlayerState(NewPlayerState);
	}

#if UE_EDITOR
	FLogger::ErrorLog("Couldn't Find Player State Component.");
#endif

	return false;
}

#pragma endregion 

void APlayerCharacter::LoadPlayerFromDataTable()
{
	if (IsValid(DataTablePtr) && IsValid(DataTableRowHandler.DataTable))
	{
		const FPlayerDataDriven* PlayerData = DataTableRowHandler.DataTable->FindRow<FPlayerDataDriven>("CH_Francesca","");
		if (PlayerData)
		{
			FLogger::DebugLog("%i", PlayerData->MaxHealth);
		}
	}
}

void APlayerCharacter::ToggleShield()
{
	SetPlayerDevice();
	if (!CanChangeToPlayerState(EPlayerState::OpenShield) || !IsValid(Umbrella))
	{
		return;
	}
	
	if (IsAiming())
	{
		Umbrella->ToggleOpenClose();

		// Opening the shield kicks the player out of a cover.
		// Needs to be done after the Umbrella open to be able to detect that we need to push the player back cause of the shield.
		if (IsValid(CoverMovementComponent) && CoverMovementComponent->IsMovementControlledByCover())
		{
			NotifyExitCover();
		}
	}
}

void APlayerCharacter::HoldShield()
{
	SetPlayerDevice();
	
	if (!CanChangeToPlayerState(EPlayerState::OpenShield))
	{
		return;
	}
	
	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}
	
	CameraSystem->SetCameraState(FCameraStateKey::AIM);
	SetPlayerGeneralState(EPlayerState::OpenShield);
	
	SetIsAiming(true);
	SetIsStrafing(true);

	if (UCharacterMovementComponent* UCharacterMovementComponent = GetCharacterMovement())
	{
		UCharacterMovementComponent->bOrientRotationToMovement = false;
		UCharacterMovementComponent->bUseControllerDesiredRotation = true;
		UCharacterMovementComponent->MaxWalkSpeed = StrafeWalkSpeed;
	}
	
	if (IsValid(AimAssistComponent))
	{
		AimAssistComponent->OnStartAiming();
	}
	
	if (IsValid(Umbrella))
	{
		Umbrella->ToggleOpenClose();
		
		// Opening the shield kicks the player out of a cover.
		// Needs to be done after the Umbrella open to be able to detect that we need to push the player back cause of the shield.
		if (IsValid(CoverMovementComponent) && CoverMovementComponent->IsMovementControlledByCover())
		{
			NotifyExitCover();
		}
	}
}

void APlayerCharacter::StopShield()
{
	SetPlayerDevice();
	
	if (!CanChangeToPlayerState(EPlayerState::FreeMovement))
	{
		return;
	}

	AMainController* PC = Cast<AMainController>(GetController());
	
	if (bIsManuallyAiming)
	{
		if (IsValid(Umbrella))
		{
			Umbrella->Close();
			SetPlayerGeneralState(EPlayerState::AimingUmbrella);
		}
		return;
	}
	
	SetIsAiming(false);
	SetPlayerGeneralState(EPlayerState::FreeMovement);
	
	if (IsValid(Umbrella))
	{
		Umbrella->Close();

		if (UWidgetComponent* EnergyBar = Umbrella->GetEnergyBarComponent())
		{
			EnergyBar->SetVisibility(false);
		}
	}
	
	if (IsMovementControlledByCover())
	{
		CameraSystem->SetCameraState(FCameraStateKey::COVER);
	}
	else
	{
		CameraSystem->SetCameraState(FCameraStateKey::DEFAULT);
	}
	
	UCharacterMovementComponent* UCharacterMovementComponent = GetCharacterMovement();
	if (IsValid(UCharacterMovementComponent))
	{
		UCharacterMovementComponent->MaxWalkSpeed = MaxWalkSpeed;
	}
	if (!bIsInCombatVolume)
	{
		SetIsStrafing(false);
		
		if (IsValid(UCharacterMovementComponent))
		{
			UCharacterMovementComponent->bOrientRotationToMovement = true;
			UCharacterMovementComponent->bUseControllerDesiredRotation = false;
		}
	}

	if (IsValid(AimAssistComponent))
	{
		AimAssistComponent->StopRotation();
		CameraSystem->ResetCameraPosition();
	}
}

void APlayerCharacter::Reload()
{
	SetPlayerDevice();
	
	if (GetPlayerStateComponent()->CanPlayerReload() && IsValid(Umbrella))
	{
		Umbrella->StartReloading();
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	SetPlayerDevice();

	if (!GetPlayerStateComponent()->CanPlayerMove() || !IsValid(Controller))
	{
		return;
	}
	
    const FVector2D MovementVector = Value.Get<FVector2D>();

    if (MovementVector.Length() < 0.1f)
    {
    	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::Move - Movement Vector is smaller than 0.1!"));
        return;
    }
    
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);
    FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    if (IsNoClip())
    {
        ForwardDirection = GetCameraSystem()->GetFollowCamera()->GetForwardVector();
    }

    const FVector CompositeDirection = ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X;

    // First check if FreeRoamCoverComponent should block movement
    bool bFreeRoamBlocking = false;
    if (IsValid(FreeRoamCoverComponent) && FreeRoamCoverComponent->ShouldBlockMovement())
    {
        bFreeRoamBlocking = true;
        
        // Allow rotation even when movement is blocked
        FRotator TargetRotation = (CompositeDirection.Rotation());
        SetActorRotation(FMath::RInterpTo(
            GetActorRotation(),
            TargetRotation,
            GetWorld()->GetDeltaSeconds(),
            FreeRoamCoverComponent->RotationInterpSpeed
        ));
    }

    // Cover System is controlling this character.
    if (IsValid(CoverMovementComponent) && (CoverMovementComponent->IsMovementControlledByCover() || CoverMovementComponent->IsTravellingToCover()))
    {
        if (!CoverMovementComponent->IsTravellingToCover() && !bFreeRoamBlocking)
        {
            float InCoverTraversingXInput = FVector::DotProduct(GetActorRightVector(), CompositeDirection);
            float InCoverTraversingYInput = FVector::DotProduct(GetActorForwardVector(), CompositeDirection);
            
            CoverMovementComponent->StoreInCoverInput(InCoverTraversingYInput, InCoverTraversingXInput);
        }
    }
    // If the cover system shouldn't be controlling the movement.
    else if (!bFreeRoamBlocking) // Only allow movement if not blocked by FreeRoam
    {
    	// Exiting cover transition.
        if (IsValid(CoverMovementComponent) && CoverMovementComponent->GetCoveredState() == ECoveredState::ExitingCover)
        {
            AddMovementInput(ForwardDirection, MovementVector.Y * UEasingLibrary::ApplyEase(CoverMovementComponent->GetExitCoverTransitionCompletePercentage(), EEaseType::InQuart));
            AddMovementInput(RightDirection, MovementVector.X * UEasingLibrary::ApplyEase(CoverMovementComponent->GetExitCoverTransitionCompletePercentage(), EEaseType::InQuart));
        }
    	// Regular input.
        else
        {
            AddMovementInput(ForwardDirection, MovementVector.Y);
            AddMovementInput(RightDirection, MovementVector.X);
        }

        CameraSystem->MoveAssistance(MovementVector, IsMovementControlledByCover());
    }
}

void APlayerCharacter::StopMovingPlayer(const FInputActionValue& Value)
{
	if (IsValid(CoverMovementComponent) && CoverMovementComponent->IsMovementControlledByCover())
	{
		CoverMovementComponent->StoreInCoverInput(0,0);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	SetPlayerDevice();
	
	if (GetPlayerGeneralState() == EPlayerState::ControlsDisabled || !IsValid(Controller))
	{
		return;
	}
	
	CameraSystem->SetCameraInMotion(true);

	// Adding Controller input.
	const FVector2D LookAxisVector = CameraSystem->GetAdjustedLookVector(GetPlayerDevice(), Value.Get<FVector2D>());
	AddControllerYawInput(LookAxisVector.X * CameraLookMultiplierAimAssist);
	AddControllerPitchInput(LookAxisVector.Y * CameraLookMultiplierAimAssist);
}

void APlayerCharacter::StopLook(const FInputActionValue& Value)
{
	if (GetPlayerGeneralState() == EPlayerState::ControlsDisabled || !IsValid(CameraSystem))
	{
		return;
	}
	
	CameraSystem->SetCameraInMotion(false);
}

void APlayerCharacter::SetPlayerDevice()
{
	FPlatformUserId PlayerID = FPlatformUserId::CreateFromInternalId(0);
	
	if (const UInputDeviceSubsystem* InputDeviceSubsystem = UInputDeviceSubsystem::Get())
	{
		FHardwareDeviceIdentifier DeviceIdentifier = InputDeviceSubsystem->GetMostRecentlyUsedHardwareDevice(PlayerID);
		FString DeviceName = DeviceIdentifier.ToString();
		TArray<FString> ParsedDeviceName;
		DeviceName.ParseIntoArray(ParsedDeviceName, TEXT("::"));

		if (ParsedDeviceName.Num() > 0)
		{
			PlayerDevice = ParsedDeviceName.Last();
		}
	}
}

AUmbrella* APlayerCharacter::GetUmbrella() const
{
	return Umbrella;
}

void APlayerCharacter::InteractingWithOverlappingElements(const FInputActionValue& Value)
{
	SetPlayerDevice();

	if (!GetPlayerStateComponent()->CanInteractWithOverlappingElements())
	{
		return;
	}
	
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);
	
	for (AActor* OverlappingActor : OverlappingActors)
	{
		// If it doesn't implement our Interact Interface, we continue to the next Actor.
		IInteractInterface* InteractInterface = Cast<IInteractInterface>(OverlappingActor);
		if (!InteractInterface)
		{
			continue;
		}
		
		if (const int32 Currency = InteractInterface->Interacting())
		{
			if (OverlappingActor->IsA(AAmmo::StaticClass()))
			{
				// set the ammo to the umbrella.
				return;
			}
			if (OverlappingActor->IsA(AUpgradePieces::StaticClass()))
			{
				CurrentPiecesForUpgrades += Currency;
				return;
			}
		}
		else if(InteractInterface->GetTypeOfInteraction() == PLAYER_CHARACTER)
		{
			InteractInterface->Interacting(this);
		}
	}
}

void APlayerCharacter::ShootingUmbrella()
{
	SetPlayerDevice();

	if (!GetPlayerStateComponent()->CanShootUmbrella())
	{
		return;
	}
	
	if (IsValid(Umbrella))
	{
		if (IsAiming())
		{
			Shoot();
		}
		else if (!IsMeleeAttacking())
		{
			StartMeleeAttack();
		}
	}
}

void APlayerCharacter::Shoot()
{
	if (!IsValid(Umbrella) || bIsElectrified)
	{
		return;
	}
	
	if (Umbrella->TryToShoot())
	{
		if (IsValid(CameraSystem))
		{
			CameraSystem->MakeCameraShake(FShakeSource::PLAYER);
		}
			
		// Play montage
		float RecoilPlayRate = 1.0f / (GetUmbrella()->GetShootingCadence() + RecoilAnimSpeedOffset);
		PlayMontage(RecoilMontage, RecoilPlayRate);
					
		// Trigger delegate
		(void)OnPlayerShot.ExecuteIfBound();
	}
}

// To be checked. Health subtract should be called from class hitting the actor to have the damage we subtract there
// instead of calling an overlap from the player
void APlayerCharacter::GetHit(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Overlap with player ") );
	if (AGenericProjectile* Projectile = Cast<AGenericProjectile>(OtherActor))
	{
		//GetComponentByClass<UC_Health>()->SubtractHealth(30);
		//HealthComponent->SubtractHealth(30);
	}
}

void APlayerCharacter::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void APlayerCharacter::GadgetShoot()
{
	const bool bCanNotShoot = GetPlayerGeneralState() != EPlayerState::AimingUmbrella && GetPlayerGeneralState() != EPlayerState::InCoverAiming && GetPlayerGeneralState() != EPlayerState::OpenShield;
	if (bCanNotShoot)
	{
		return;
	}
	
	if (IsValid(Umbrella) && IsAiming())
	{
		Umbrella->GadgetShoot();
	}
}

bool APlayerCharacter::IsMeleeAttacking() const
{
	return bIsMeleeAttacking;
}

#pragma region SAVE FUNCTIONS IMPLEMENTATION

void APlayerCharacter::SaveGame()
{
	// Set the GameInstanceData from the player for it to be saved into the SaveGameObject and save the game
	ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance());
	if (IsValid(GameInstance))
	{
		// Gives me the chills, better to have it implemented in the player
		// GameInstance->SetGI_Player_Data(PlayerSaveStruct.health);
		GameInstance->GI_SaveStructPlayer.Health = PlayerSaveStruct.Health;
		GameInstance->GI_SaveStructPlayer.WorldTransform = GetController()->GetLevelTransform();
	}
}

void APlayerCharacter::LoadGame()
{
	// Search for the GameInstanceData from the player loaded from the SaveGameObject and set it into the player
	ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance());
	if (IsValid(GameInstance))
	{
		/*
		PlayerSaveStruct.Health = GameInstance->GI_SaveStructPlayer.Health;
		PlayerSaveStruct.WorldTransform = GameInstance->GI_SaveStructPlayer.WorldTransform;
		*/
		HealthComponent->SetHealth(GameInstance->GI_SaveStructPlayer.Health);
		SetActorTransform(GameInstance->GI_SaveStructPlayer.WorldTransform);
	}
}

void APlayerCharacter::SaveDataCheckPoint()
{
	ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance());
	
	if (IsValid(GameInstance))
	{
		// FLogger::DebugLog("Saving data into the GameInstance");
		if (IsValid(HealthComponent))
		{
			GameInstance->GI_SaveStructPlayer.Health = HealthComponent->GetHealth();
		}
		
		// Might not need to use this transform
		GameInstance->GI_SaveStructPlayer.WorldTransform = GetActorTransform();

		if (IsValid(Umbrella))
		{
			GameInstance->GI_SaveStructPlayer.UmbrellaCurrentMagAmmo = Umbrella->GetAmmoCurrent();
			GameInstance->GI_SaveStructPlayer.UmbrellaInventoryAmmo  = Umbrella->GetAmmoReserve();
		}
	}
}

void APlayerCharacter::LoadDataCheckPoint()
{
	ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance());
	
	if (IsValid(GameInstance))
	{
		// If this runs before loading the player kaboom
		// FLogger::DebugLog("APlayerCharacter::LoadDataCheckPoint");
		ACheckPointTrigger* LastCheckpointHit = GameInstance->GetLastCheckpointHit();		

		if (IsValid(LastCheckpointHit))
		{
			SetActorTransform(LastCheckpointHit->GetArrowTransform());
			//GetActorForwardVector() = GameInstance->LastCheckpointHit->GetArrowForwardVector();
			GetController()->SetControlRotation(GetActorForwardVector().Rotation());
		}
		
		if (IsValid(HealthComponent))
		{
			// For now stays like this
			HealthComponent->SetHealth(GameInstance->GI_SaveStructPlayer.Health);
		}
		
		if (IsValid(Umbrella))
		{
			Umbrella->SetAmmoCurrent(GameInstance->GI_SaveStructPlayer.UmbrellaCurrentMagAmmo);
			Umbrella->SetAmmoReserve(GameInstance->GI_SaveStructPlayer.UmbrellaInventoryAmmo);
			Umbrella->SetHealth(100);

			(void) Umbrella->OnCurrentBulletsChanged.ExecuteIfBound(Umbrella->GetAmmoCurrent());
			(void) Umbrella->OnTotalBulletsChanged.ExecuteIfBound(Umbrella->GetAmmoReserve());
		}
	}
}

#pragma endregion

void APlayerCharacter::CheckMantle()
{
	if (!IsInZipline()) // do not check when in a zipline
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Init(this, 1);

		if (IsValid(Umbrella))
		{
			IgnoredActors.Add(Umbrella);
		}

		InteractiveMovementComponent->CheckMantle(IgnoredActors);
	}
}

bool APlayerCharacter::CheckVault()
{
	if (IsValid(CoverMovementComponent) && CoverMovementComponent->IsMovementControlledByCover())
	{
		NotifyExitCover();
	}
	
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Init(this, 1);

	if (IsValid(Umbrella))
	{
		IgnoredActors.Add(Umbrella);
	}

	if (InteractiveMovementComponent->CheckVault(IgnoredActors))
	{
		ResetAiming();
		return true;
	}

	return false;
}

bool APlayerCharacter::CheckSwing()
{
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Init(this, 1);

	if (IsValid(Umbrella))
	{
		IgnoredActors.Add(Umbrella);
	}

	if (InteractiveMovementComponent->CheckSwing(IgnoredActors))
	{
		return true;
	}

	return false;
}

void APlayerCharacter::Jump()
{
	SetPlayerDevice();

	// We can only jump/vault while in regular movement or while in cover.
	if (GetPlayerGeneralState() != EPlayerState::FreeMovement
		&& GetPlayerGeneralState() != EPlayerState::InCover)
	{
		return;
	}
	
	if (!GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->IsFlying() && !SetIsAiming(false))
	{
		// We try to Swing and Vault before jumping.
		if (!CheckSwing() && !CheckVault())
		{
			ACharacter::Jump();
			// SpeakRandomActionLine(EActionCode::VAULT);
		}
	}
}

void APlayerCharacter::CycleUmbrellaBullets()
{
	SetPlayerDevice();

	// We disable umbrella cycling in certain states.
	if (GetPlayerGeneralState() == EPlayerState::ControlsDisabled
		|| GetPlayerGeneralState() == EPlayerState::LimitedMovement)
	{
		return;
	}
	
	if (IsValid(Umbrella))
	{
		Umbrella->CycleBulletType();
	}
}

bool APlayerCharacter::CanSwapGadget() const
{
	// We disable umbrella cycling in certain states.
	if (GetPlayerGeneralState() == EPlayerState::ControlsDisabled
		|| GetPlayerGeneralState() == EPlayerState::LimitedMovement)
	{
		return false;
	}
	return true;
}
void APlayerCharacter::SwapToHook()
{
	SetPlayerDevice();

	if (!CanSwapGadget())
	{
		return;
	}
	
	if (IsValid(Umbrella))
	{
		Umbrella->SetBulletType(EGadgetType::Grapple);
	}
}

void APlayerCharacter::SwapToElectric()
{
	SetPlayerDevice();
	
	if (!CanSwapGadget())
	{
		return;
	}
	
	if (IsValid(Umbrella))
	{
		Umbrella->SetBulletType(EGadgetType::Electric);
	}
}

void APlayerCharacter::MovePlayerViaGrapple(const FVector& EndPosition)
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying, 0);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	SetIsGrappling(true);
	
	FTimerDelegate Grappling_Delegate; // Delegate to bind function with parameters
	Grappling_Delegate.BindUFunction(this, "Grappling", EndPosition); // Character is the parameter we wish to pass with the function.
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_GrapplingDelay, Grappling_Delegate, GetWorld()->DeltaTimeSeconds, true);
}

void APlayerCharacter::Grappling(const FVector& EndPosition)
{
	if (IsGrappling())
	{
		UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
		const FVector ActorLocation = GetActorLocation();
		MovementComponent->Velocity = 2500 * UKismetMathLibrary::GetDirectionUnitVector(ActorLocation, EndPosition);
		
		const FVector CalculusVector = ActorLocation - EndPosition;
		Umbrella->SetCableEndLocation( UKismetMathLibrary::InverseTransformLocation(Umbrella->GetActorTransform(), GetActorLocation()));

		if (UKismetMathLibrary::NearlyEqual_FloatFloat(CalculusVector.Length(),0,100))
		{
			MovementComponent->StopMovementImmediately();
			SetIsGrappling(false);
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_GrapplingDelay);

			Umbrella->GetCableComponent()->SetVisibility(false);
			Umbrella->GetCableComponent()->SetComponentTickEnabled(false);
		}
	}
}

#pragma region MELEE ATTACK

void APlayerCharacter::StartMeleeAttack()
{
	// If we are in cover, we exit it.
	if (IsValid(CoverMovementComponent) && CoverMovementComponent->IsMovementControlledByCover())
	{
		CoverMovementComponent->NotifyExitCover();
	}
	
	SetPlayerGeneralState(EPlayerState::InMeleeAnimation);
	
	SetIsMeleeAttacking(true);
	if(IsValid(GetMesh()))
	{
		PlayMontage(MeleeMontage, 1);
	}
					
	FVector V_MeleePos = CheckEnemyInMeleeRange();
	if (V_MeleePos != FVector::ZeroVector)
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), V_MeleePos);
		FRotator PlayerRotation = GetActorRotation();
		TargetMeleeRot = FRotator(PlayerRotation.Pitch, LookAtRotation.Yaw, PlayerRotation.Roll);
		if (IsValid(MeleeTimeLine))
		{
			MeleeTimeLine->PlayFromStart();
		}
	}
}

void APlayerCharacter::MeleeTimelineProgress(float Value)
{
	FRotator CurrentRot = FMath::Lerp(this->GetActorRotation(), TargetMeleeRot, Value);
	SetActorRotation(CurrentRot);
}

FVector APlayerCharacter::CheckEnemyInMeleeRange()
{
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	// For first playable - may be subject to change
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACoverParent::StaticClass(), ActorsToIgnore); // THIS WILL LIKELY NEED TO BE CHANGED
	ActorsToIgnore.Add(this);
	FVector StartMeleeLocation = GetActorLocation();
	ETraceTypeQuery Channel = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Pawn);

	bool bIsHit = UKismetSystemLibrary::SphereTraceSingle(
		this,
		StartMeleeLocation,
		StartMeleeLocation,
		MeleeRange,
		Channel,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::Type::None,
		HitResult,
		true
	);
	
	if (bIsHit)
	{
		if (UHealthComponent* HealthComp = HitResult.GetActor()->GetComponentByClass<UHealthComponent>())
		{
			FVector HitLocation = HitResult.GetActor()->GetActorLocation();
			UE_LOG(LogTemp, Warning, TEXT("Detected Actor: %s"), *HitResult.GetActor()->GetName());
			// DrawDebugSphere(GetWorld(), HitLocation, 50.f, 12, FColor::Red, false, 5.f);

			return HitLocation;
		}
	}
	
	return FVector::ZeroVector;
}

void APlayerCharacter::PlayGrappleAnim() const
{	
	PlayMontage(GrapplePullMontage, GrappleMontagePlayRate);
}

#pragma endregion