// Copyright (c) 2019–2020 Alden Wu

using UnrealBuildTool;
using System.Collections.Generic;

public class PaperEditorTarget : TargetRules
{
    public PaperEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange(new string[] {
			"Paper"
		});
	}
}
