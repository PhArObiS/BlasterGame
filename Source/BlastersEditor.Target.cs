// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class BlastersEditorTarget : TargetRules
{
	public BlastersEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		// DefaultBuildSettings = BuildSettingsVersion.V4;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;

		ExtraModuleNames.Add("Blasters");
	}
}
