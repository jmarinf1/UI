// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkBenchMenu.h"

#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Interfaces/UmbrellaUpgradeInterface.h"
#include "Components/Button.h"
#include "LadyUmbrella/Weapons/Umbrella/UpgradeComponent/UpgradeComponent.h"
#include "LadyUmbrella/UI/Menus/WorkbenchElements/UpgradeSlot.h"

void UWorkBenchMenu::NativeConstruct()
{
	Super::NativeConstruct();

	ExitButton->OnPressed.AddDynamic(this, &UWorkBenchMenu::CloseMenu);
	Button_Left->OnPressed.AddDynamic(this, &UWorkBenchMenu::OnClickButton);
	Button_Right->OnPressed.AddDynamic(this, &UWorkBenchMenu::OnClickButton);
	
}

void UWorkBenchMenu::SetPiecesElement(const int32 NewPieces) const
{
	// Leer de la variable de clase en lugar de por parametro
	FNumberFormattingOptions Options;
	Options.MinimumIntegralDigits = 3;
	AmountOfPlayerPieces->SetText(FText::AsNumber(NewPieces, &Options));
}


void UWorkBenchMenu::ReadUmbrellaUpgrades(const TMap<EUpgradeType, FUpgrade>& Upgrades)
{
	int ID = 0;

	for (auto It = Upgrades.CreateConstIterator(); It; ++It, ++ID)
	{
		UUpgradeSlot* NewSlot = CreateWidget<UUpgradeSlot>(GetWorld(), UpgradeSlotClass);
		if (IsValid(NewSlot))
		{
			// El upgrade slot necesita saber el nivel, el max level y el summary, no necesita el componente entero.
			// Recibe el TPair del mapa
			NewSlot->SetupUpgradeSlots(*It);
			NewSlot->SetUpgradeId(ID);
			NewSlot->ProgressEndedDelegate.BindUObject(this, &UWorkBenchMenu::OnUpgradePressed);
			UpgradeSlotArray.Add(NewSlot);
			
			if (IsValid(VB_Upgrades))
			{
				if (ID < Upgrades_Separator )
				{
					VB_Upgrades_Shield->AddChild(NewSlot);
				}
				else
				{
					VB_Upgrades_Shotgun->AddChild(NewSlot);
				}
			}
		}
	}
	VB_Upgrades_Shotgun->SetVisibility(ESlateVisibility::Hidden);
}

void UWorkBenchMenu::OnUpgradePressed(const int32 UpgradePressed, const int32 CurrentUpgrade)
{
	TArray<AActor*> UmbrellaActor;

	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UUmbrellaUpgradeInterface::StaticClass(), UmbrellaActor  );
	// I don't like having the Player here.
	APlayerCharacter* Player = Cast<APlayerCharacter>( UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	AActor* Umbrella = UmbrellaActor[0];
	
	if (IsValid(Umbrella))
	{
		if (IUmbrellaUpgradeInterface* UpgradeComponentInterface = Cast<IUmbrellaUpgradeInterface>(Umbrella))
		{
			UUpgradeComponent* UpgradeComponent = UpgradeComponentInterface->GetUmbrellaUpgradeComponent();
			if (IsValid(UpgradeComponent))
			{
				FUpgrade* Upgrade  = UpgradeComponent->Upgrades.Find(static_cast<EUpgradeType>(UpgradePressed));
				int32 UpgradePrice = Upgrade->GetPrice();
				// La mejora del widget tiene que ser inferior al max level del component
				if (UpgradePrice  < PiecesOfThePlayer && CurrentUpgrade < Upgrade->GetMaxLevel() )
				{
					if (IsValid(UpgradeSlotArray[UpgradePressed]))
					{
						const int32 CurrentLevel = Upgrade->GetLevel();
						PiecesOfThePlayer -= UpgradePrice;
						Player->SetNewCurrentPiecesForUpgrades(PiecesOfThePlayer);
						SetPiecesElement(PiecesOfThePlayer);
						
						UpgradeSlotArray[UpgradePressed]->SetPercentToBar(CurrentLevel);
						Upgrade->SetLevel(CurrentLevel + 1);
					}
				}
			}
		}
	}
}

void UWorkBenchMenu::CloseMenu()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	APlayerCharacter* PlayerCharacter	= Cast<APlayerCharacter>( UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (IsValid(PlayerController))
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetShowMouseCursor(false);
		PlayerController->SetViewTargetWithBlend(PlayerCharacter, 0.5f, VTBlend_Linear, 0, false);
		SetVisibility(ESlateVisibility::Hidden);
		//RemoveFromParent();
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
// No const bc has to be assigned from Delegate. Rider shut up.
void UWorkBenchMenu::OnClickButton()
{
	if (IsValid(VB_Upgrades_Shield) && IsValid(VB_Upgrades_Shotgun))
	{
		if (VB_Upgrades_Shield->IsVisible())
		{
			VB_Upgrades_Shield->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			VB_Upgrades_Shield->SetVisibility(ESlateVisibility::Visible);
		}

		if (VB_Upgrades_Shotgun->IsVisible())
		{
			VB_Upgrades_Shotgun->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			VB_Upgrades_Shotgun->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UWorkBenchMenu::OpenMenu() const
{
	if (IsValid(VB_Upgrades_Shield) && IsValid(VB_Upgrades_Shotgun))
	{
		VB_Upgrades_Shield->SetVisibility(ESlateVisibility::Visible);
		VB_Upgrades_Shotgun->SetVisibility(ESlateVisibility::Hidden);
	}		
}