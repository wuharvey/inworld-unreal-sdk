/**
 * Copyright 2022-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

using UnrealBuildTool;

public class InworldRPMEditor : ModuleRules
{
	public InworldRPMEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"glTFRuntime",
				"glTFRuntimeEditor"
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
            {
                "InworldAIEditor",
                "InworldAIClient",
                "InworldAIIntegration",
				"CoreUObject",
				"Engine",
			}
			);
	}
}
