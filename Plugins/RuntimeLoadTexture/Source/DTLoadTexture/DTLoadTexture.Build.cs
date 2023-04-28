// Copyright 2022-2023 Dexter.Wan. All Rights Reserved. 
// EMail: 45141961@qq.com


using UnrealBuildTool;

public class DTLoadTexture : ModuleRules
{
	public DTLoadTexture(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange( new string[] {} );
		PrivateIncludePaths.AddRange( new string[] {} );
		PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"InputCore",
					"HTTP"
				}
			);
		PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"CoreUObject",
					"Engine",
				}
			);
		DynamicallyLoadedModuleNames.AddRange( new string[]{} );
	}
}
