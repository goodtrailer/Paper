// Copyright (c) 2020 Alden Wu

using System;
using System.IO;
using UnrealBuildTool;

public class stb : ModuleRules
{
	public stb(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
		
		PublicDefinitions.Add("STB_IMAGE_IMPLEMENTATION");
		PublicDefinitions.Add("STBI_NO_FAILURE_STRINGS");
		
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Include"));
	}
}