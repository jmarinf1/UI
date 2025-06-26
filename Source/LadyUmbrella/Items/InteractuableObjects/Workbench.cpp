/*
 * Zulo Interactive
 */

#include "Workbench.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Components/SphereComponent.h"
#include "LadyUmbrella/UI/Basics/InteractiveIcon.h"
#include "LadyUmbrella/UI/Menus/WorkbenchMenu/WorkBenchMenu.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"

AWorkbench::AWorkbench()
{
	CameraComponent		  = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	StaticMeshComponent	  = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Main Mesh"));
	
	CameraComponent->SetupAttachment(GetSphereComponent());
	StaticMeshComponent->SetupAttachment(GetSphereComponent());
	
	ConstructorHelpers::FClassFinder<UInteractiveIcon> WidgetFinder(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/Components/WBP_InteractiveIcons.WBP_InteractiveIcons_C'"));

	if (WidgetFinder.Succeeded())
	{
		SetWidgetClass(WidgetFinder.Class);
	}
}

void AWorkbench::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
}

// void AWorkbench::OnConstruction(const FTransform& Transform)
// {
// 	Super::OnConstruction(Transform);
// 	if (IsValid(CollisionSphereComponent))
// 	{
// 		CollisionSphereComponent->SetSphereRadius(SphereComponentRadius);
// 	}
// }

int32 AWorkbench::Interacting()
{
	DisableInputCustom();
	SetUpWidget();
	// Does not give anything back (ammo or pieces)
	return 0;
}

int32 AWorkbench::Interacting(APlayerCharacter* PlayerCharacternInteractor)
{
	return 0;
}

void AWorkbench::DisableInputCustom()
{
	// Disable input
	PlayerController->SetViewTargetWithBlend(this, 0.5f, VTBlend_Linear, 0, false);
	//PlayerController->SetInputMode(FInputModeUIOnly());
	PlayerController->SetInputMode(FInputModeUIOnly());
	PlayerController->FlushPressedKeys();
	PlayerController->SetShowMouseCursor(true);
	//PlayerController->Pause();
}

void AWorkbench::SetUpWidget()
{
	if (IsValid(WorkbenchWidgetReference))
	{
		if (!WorkbenchCreatedWidget)
		{
			WorkbenchCreatedWidget = CreateWidget<UWorkBenchMenu>(GetWorld(), WorkbenchWidgetReference);

			if (IsValid(WorkbenchCreatedWidget))
			{
				WorkbenchCreatedWidget->AddToViewport();
				// TODO: Hide the UI of the player. We don't need the ammo. There is a function that does that in our PlayerController
				// Here we should play anim of dropping the umbrella into the workbench or smth like that
				// TODO: Create an interface to bring the amount of pieces if that's the only thing needed, we avoid Entire PlayerClass and save a cast to the player
				AUmbrella* Umbrella = PlayerCharacter->GetUmbrella();
				if (IsValid(PlayerCharacter) && IsValid(Umbrella))
				{
					// WorkbenchWidget Setup
					// WorkbenchCreatedWidget->PlayFadeIn();
					// Set UI For pieces
					PiecesForUpgrade = PlayerCharacter->GetCurrentPiecesForUpgrades();
					WorkbenchCreatedWidget->SetVisibility(ESlateVisibility::Visible);
					WorkbenchCreatedWidget->SetPiecesOfThePlayer(PiecesForUpgrade);
					WorkbenchCreatedWidget->SetPiecesElement(PiecesForUpgrade);
					WorkbenchCreatedWidget->ReadUmbrellaUpgrades(Umbrella->GetUpgradeComponent()->GetMap());
				}
			}
		}
		else
		{
			PiecesForUpgrade = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0))->GetCurrentPiecesForUpgrades();
			WorkbenchCreatedWidget->SetVisibility(ESlateVisibility::Visible);
			WorkbenchCreatedWidget->OpenMenu();
			WorkbenchCreatedWidget->SetPiecesElement(PiecesForUpgrade);
			WorkbenchCreatedWidget->SetPiecesOfThePlayer(PiecesForUpgrade);
		}
	}
}
