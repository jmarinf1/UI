// Fill out your copyright notice in the Description page of Project Settings.


#include "Beretta.h"

ABeretta::ABeretta()
{
	static const wchar_t* SkeletalMeshAsset = TEXT("/Script/Engine.SkeletalMesh'/Game/Meshes/SkeletalMeshes/SK_Beretta.SK_Beretta'");
	
	SetAmmoCurrent(6);
	SetAmmoCurrentMax(6);
	SetAmmoReserve(12);
	SetAmmoReserveMax(12);
	SetWeaponCadence(1.0f);

	GetSkeletalMeshComponent()->SetSkeletalMesh(ConstructorHelpers::FObjectFinder<USkeletalMesh>(SkeletalMeshAsset).Object);
}
