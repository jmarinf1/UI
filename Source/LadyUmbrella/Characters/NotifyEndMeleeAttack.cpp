#include "NotifyEndMeleeAttack.h"

#include "Player/PlayerCharacter.h"

void UNotifyEndMeleeAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!IsValid(MeshComp))
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Owner);
	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	PlayerCharacter->SetPlayerGeneralState(EPlayerState::FreeMovement);
}
