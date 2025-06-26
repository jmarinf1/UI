#include "ANotify_CanAttackMelee.h"
#include "LadyUmbrella/Interfaces/GetWeaponInterface.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "LadyUmbrella/Weapons/Umbrella/MeleeComponent/MeleeComponent.h"
#include "Player/PlayerCharacter.h"

void UANotify_CanAttackMelee::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!IsValid(MeshComp))
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	// Check if the owner implements the GetWeaponInterface
	if (Owner->Implements<UGetWeaponInterface>())
	{
		AUmbrella* Umbrella = Cast<IGetWeaponInterface>(Owner)->GetWeapon();
		if (IsValid(Umbrella))
		{
			//TODO: Remove this cast and reset value using interface
			if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Owner))
			{
				PlayerCharacter->SetIsMeleeAttacking(false);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Owner is not a PlayerCharacter"));
			}
			Umbrella->GetMeleeComponent()->SetCanDetectMelee(true);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Owner does not implement GetWeaponInterface"));
	}
}
