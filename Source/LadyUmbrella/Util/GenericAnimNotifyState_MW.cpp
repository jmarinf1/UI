// Fill out your copyright notice in the Description page of Project Settings.


#include "GenericAnimNotifyState_MW.h"

#include "RootMotionModifier.h"
#include "LadyUmbrella/Characters/InteractiveMovementComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"

void UGenericAnimNotifyState_MW::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	//UE_LOG(LogTemp, Warning, TEXT("MotionWarp Notify BEGIN"));

	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Owner))
		{
			PlayerCharacter->GetInteractiveMovementComponent()->OnMotionWarpStarted(NotifyName);
		}
	}
}

void UGenericAnimNotifyState_MW::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//UE_LOG(LogTemp, Warning, TEXT("MotionWarp Notify FINISH"));
	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Owner))
		{
			PlayerCharacter->GetInteractiveMovementComponent()->OnMotionWarpFinished(NotifyName);
		}
	}
}