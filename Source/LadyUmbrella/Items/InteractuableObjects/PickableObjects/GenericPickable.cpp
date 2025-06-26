// Fill out your copyright notice in the Description page of Project Settings.


#include "GenericPickable.h"

int32 AGenericPickable::Interacting()
{
	Super::Interacting();
	
	return 0;
}

int32 AGenericPickable::Interacting(APlayerCharacter* Interactor)
{
	Super::Interacting(Interactor);
	
	return 0;
}
