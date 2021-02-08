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

		PublicDefinitions.Add("ALLOW_CONSOLE_IN_SHIPPING=1");

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"HeadMountedDisplay",
			"stb",
			"pfd"
		});
	}
}
