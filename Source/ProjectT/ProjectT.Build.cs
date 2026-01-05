// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectT : ModuleRules
{
	public ProjectT(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{ 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"GameplayCameras",
			"GameplayTags",
			"GameplayTasks",
			"GameplayAbilities",
            "Niagara",
            "AIModule",
            "NavigationSystem",
            "UMG",
            "AnimGraphRuntime",
        });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        PublicIncludePaths.AddRange(new string[] 
		{
            "ProjectT",
            "ProjectT/Player",
            "ProjectT/GAS",
            "ProjectT/Input",
            "ProjectT/Manager",
            "ProjectT/Monster",
            "ProjectT/UI",
            "ProjectT/Component",
        });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
