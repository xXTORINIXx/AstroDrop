using UnrealBuildTool;

public class MultiplayerSessions : ModuleRules
{
	public MultiplayerSessions(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"NetCore",
				"Networking",
				"OnlineSubsystem",
				"OnlineSubsystemUtils",
				"OnlineSubsystemEOS",
				"Voice",
				"UMG",
				"Slate",
				"SlateCore"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"ApplicationCore"
			}
		);
	}
}
