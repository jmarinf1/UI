﻿// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class LadyUmbrellaEditor : ModuleRules
{
	public LadyUmbrellaEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"UnrealEd",
			"DetailCustomizations",
			"PropertyEditor",
			"EditorStyle",
			"LadyUmbrella"
			});
		
		PrivateDependencyModuleNames.AddRange(
			new string[] { 
			"Slate",
			"SlateCore"
		});
	}
}