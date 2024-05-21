// Copyright Developer Bastian 2024. Contact: developer.bastian@gmail.com or https://discord.gg/8JStx9XZGP. License Creative Commons 4.0 DEED (https://creativecommons.org/licenses/by/4.0/deed.en).

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
