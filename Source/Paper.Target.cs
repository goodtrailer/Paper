// Copyright (c) 2019–2020 Alden Wu

using UnrealBuildTool;
using System.Collections.Generic;

public class PaperTarget : TargetRules
{
    public PaperTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.Add("Paper");
	}
}
