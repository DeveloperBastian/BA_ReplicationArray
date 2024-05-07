// Copyright Developer Bastian 2024. Contact developer.bastian@gmail.com. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class BA_RepArrayDemoEditorTarget : TargetRules
{
	public BA_RepArrayDemoEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("BA_RepArrayDemo");
	}
}
