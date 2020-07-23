// Copyright (c) 2019–2020 Alden Wu

using System;
using System.IO;
using UnrealBuildTool;

public class Paper : ModuleRules
{
	public Paper(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.Add(ModuleDirectory);

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"HeadMountedDisplay"
		});
	}
}
