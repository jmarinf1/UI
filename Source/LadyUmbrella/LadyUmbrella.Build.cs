// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class LadyUmbrella : ModuleRules
{
	public LadyUmbrella(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
			"AIModule", "NavigationSystem", "CableComponent", "UMG", "Niagara", "MotionWarping", "RenderCore", "SlateCore", "GameplayTags" });
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
		}
	}
}
