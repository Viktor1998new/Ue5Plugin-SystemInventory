// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class InventoryEditor : ModuleRules
{
	public InventoryEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Inventory",
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"Kismet",
				"EditorStyle",
				"Slate",
				"UMG",
				"SlateCore",
				"MainFrame",
                "UMGEditor",
                "AssetTools",
                "UnrealEd",
                "Json",
                "JsonUtilities",
            }
		);

		PrivateIncludePaths.AddRange(
			new string[] {
				"InventoryEditor/Private",
			}
		);

		var DynamicModuleNames = new string[] {
			"LevelEditor",
			"PropertyEditor",
		};

		foreach (var Name in DynamicModuleNames)
		{
			PrivateIncludePathModuleNames.Add(Name);
			DynamicallyLoadedModuleNames.Add(Name);
		}
	}
}
