#include "ANS_CheckMelee.h"
#include "LadyUmbrella/Interfaces/GetWeaponInterface.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "LadyUmbrella/Weapons/Umbrella/MeleeComponent/MeleeComponent.h"

void UANS_CheckMelee::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    if (!MeshComp)
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
            Umbrella->GetMeleeComponent()->ProcessCollisionMelee(Umbrella->GetActorLocation(),Umbrella->GetActorRightVector());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Owner does not implement GetWeaponInterface"));
    }
}
