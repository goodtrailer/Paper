// Copyright (c) 2019–2020 Alden Wu

using System;
using System.IO;
using UnrealBuildTool;

public class EOSEdEngine : ModuleRules
{
	public EOSEdEngine(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"HeadMountedDisplay",
			"Paper",
			"EpicOnlineServicesSDK",
			"UnrealEd"
		});
	}
}
