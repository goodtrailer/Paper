// Copyright (c) 2020 Alden Wu

using System;
using System.IO;
using UnrealBuildTool;

public class pfd : ModuleRules
{
	public pfd(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Include"));
	}
}