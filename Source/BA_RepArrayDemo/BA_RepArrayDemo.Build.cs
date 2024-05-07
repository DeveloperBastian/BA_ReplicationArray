// Copyright Developer Bastian 2024. Contact developer.bastian@gmail.com. All Rights Reserved.

using UnrealBuildTool;

public class BA_RepArrayDemo : ModuleRules
{
	public BA_RepArrayDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
