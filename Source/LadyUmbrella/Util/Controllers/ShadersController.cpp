// 
// ShadersController.cpp
// 
// Implementation of the ShadersController class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#include "LadyUmbrella/Util/Controllers/ShadersController.h"
#include "LadyUmbrella/UI/Basics/LoadingScreen.h"
#include "ShaderPipelineCache.h"
#include "Kismet/GameplayStatics.h"

AShadersController::AShadersController()
{
	PrimaryActorTick.bCanEverTick = true;
	LoadingScreenInstance = nullptr;
	//For showcasing the Loading screen for 5 seconds
	ElapsedTime = 0.0f;
	DisplayTime = 5.0;
	//
}


void AShadersController::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(LoadingScreenWidget))
	{
		LoadingScreenInstance = CreateWidget<ULoadingScreen>(GetWorld(), LoadingScreenWidget);
		if (IsValid(LoadingScreenInstance))
		{
			LoadingScreenInstance->AddToViewport();
			LoadingScreenInstance->SetMaximumValue(71.0f); // Max numbers of PSOs when cooked
		}
	}

	// Begin compiling shaders
	FShaderPipelineCache::ResumeBatching();
}

void AShadersController::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;

	const int32 Remaining = FShaderPipelineCache::NumPrecompilesRemaining();
	if (IsValid(LoadingScreenInstance))
	{
		const float Compiled = LoadingScreenInstance->GetMaximumValue() - static_cast<float>(Remaining);
		LoadingScreenInstance->UpdateProgressBar(Compiled);

		if (!FShaderPipelineCache::IsPrecompiling() && ElapsedTime >= DisplayTime)
		{
			LoadingScreenInstance->RemoveFromParent();
			LoadingScreenInstance = nullptr;
			UGameplayStatics::OpenLevel(GetWorld(), TEXT("L_MainMenu"));
		}
	}
}

