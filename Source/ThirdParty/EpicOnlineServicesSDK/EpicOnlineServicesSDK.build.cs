// Copyright (c) 2019–2020 Alden Wu

using System;
using System.IO;
using UnrealBuildTool;

public class EpicOnlineServicesSDK : ModuleRules
{
	public EpicOnlineServicesSDK(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
		
		PublicDefinitions.Add("WITH_EPICONLINESERVICESSDK=1");
		
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Include"));

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicAdditionalLibraries.Add(Path.Combine("$(ModuleDir)", "Lib", "EOSSDK-Win64-Shipping.lib"));
			RuntimeDependencies.Add(Path.Combine("$(BinaryOutputDir)", "EOSSDK-Win64-Shipping.dll"), Path.Combine("$(ModuleDir)", "Bin", "EOSSDK-Win64-Shipping.dll"));
//			PublicDelayLoadDLLs.Add("EOSSDK-Win64-Shipping.dll");
		}
		else if (Target.Platform == UnrealTargetPlatform.Win32)
		{
			PublicAdditionalLibraries.Add(Path.Combine("$(ModuleDir)", "Lib", "EOSSDK-Win32-Shipping.lib"));
			RuntimeDependencies.Add(Path.Combine("$(BinaryOutputDir)", "EOSSDK-Win32-Shipping.dll"), Path.Combine("$(ModuleDir)", "Bin", "EOSSDK-Win32-Shipping.dll"), StagedFileType.NonUFS);
//			PublicDelayLoadDLLs.Add("EOSSDK-Win32-Shipping.dll");
		}
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			PublicAdditionalLibraries.Add(Path.Combine("$(ModuleDir)", "Bin", "libEOSSDK-Linux-Shipping.so"));
			RuntimeDependencies.Add(Path.Combine("$(BinaryOutputDir)", "libEOSSDK-Linux-Shipping.so"), Path.Combine("$(ModuleDir)", "Bin", "libEOSSDK-Linux-Shipping.so"), StagedFileType.NonUFS);
		}
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			PublicAdditionalLibraries.Add(Path.Combine("$(ModuleDir)", "Bin", "libEOSSDK-Mac-Shipping.dylib"));
			RuntimeDependencies.Add(Path.Combine("$(BinaryOutputDir)", "libEOSSDK-Mac-Shipping.dylib"), Path.Combine("$(ModuleDir)", "Bin", "libEOSSDK-Mac-Shipping.dylib"), StagedFileType.NonUFS);
		}
	}
}
