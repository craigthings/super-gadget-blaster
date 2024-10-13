// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StrikerWS : ModuleRules
{
	protected virtual bool PlatformSupportsLibWebSockets
	{
		get
		{
			return
				Target.Platform == UnrealTargetPlatform.Win64 ||
				Target.Platform == UnrealTargetPlatform.Android ||
				Target.Platform == UnrealTargetPlatform.Mac ||
				Target.IsInPlatformGroup(UnrealPlatformGroup.Unix) ||
				Target.Platform == UnrealTargetPlatform.IOS;
		}
	}

	protected virtual bool bPlatformSupportsWinHttpStrikerWS
	{
		get
		{
			// Availability requires Windows 8.1 or greater, as this is the min version of WinHttp that supports StrikerWS
			return Target.Platform.IsInGroup(UnrealPlatformGroup.Windows) && Target.WindowsPlatform.TargetWindowsVersion >= 0x0603;
		}
	}

	protected virtual bool bPlatformSupportsWinRTStrikerWS
	{
		get => false;
	}

	protected virtual bool UsePlatformSSL
	{
		get => false;
	}

	protected virtual bool ShouldUseModule
	{
		get
		{
			return PlatformSupportsLibWebSockets || bPlatformSupportsWinRTStrikerWS || bPlatformSupportsWinHttpStrikerWS;
		}
	}

	protected virtual string WebSocketsManagerPlatformInclude
	{
		get
		{
			if (PlatformSupportsLibWebSockets)
			{
				return "Lws/LwsWebSocketsManager.h";
			}
			else if (bPlatformSupportsWinRTStrikerWS)
			{
				return "WinHttp/WinHttpWebSocketsManager.h";
			}
			else
			{
				return "";
			}
		}
	}

	protected virtual string WebSocketsManagerPlatformClass
	{
		get
		{
			if (PlatformSupportsLibWebSockets)
			{
				return "FLwsWebSocketsManager";
			}
			else if (bPlatformSupportsWinRTStrikerWS)
			{
				return "FWinHttpWebSocketsManager";
			}
			else
			{
				return "";
			}
		}
	}

	public StrikerWS(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"HTTP"
			}
		);

		bool bWithStrikerWS = false;
		bool bWithLibStrikerWS = false;
		bool bWithWinHttpStrikerWS = false;

		if (ShouldUseModule)
		{
			bWithStrikerWS = true;

			if (PlatformSupportsLibWebSockets)
			{
				bWithLibStrikerWS = true;

				if (UsePlatformSSL)
				{
					PrivateDefinitions.Add("WITH_SSL=0");
					AddEngineThirdPartyPrivateStaticDependencies(Target, "libWebSockets");
				}
				else
				{
					AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenSSL", "libWebSockets", "zlib");
					PrivateDependencyModuleNames.Add("SSL");
				}
			}
			else if (bPlatformSupportsWinRTStrikerWS)
			{
				// Enable WinHttp Support
				bWithWinHttpStrikerWS = true;

				AddEngineThirdPartyPrivateStaticDependencies(Target, "WinHttp");

				// We need to access the WinHttp folder in HTTP
				PrivateIncludePaths.AddRange(
					new string[] {
						"Runtime/Online/HTTP/Private",
					}
				);
			}
		}

		PublicDefinitions.Add("WEBSOCKETS_PACKAGE=1");
		PublicDefinitions.Add("WITH_WEBSOCKETS=" + (bWithStrikerWS ? "1" : "0"));
		PublicDefinitions.Add("WITH_LIBWEBSOCKETS=" + (bWithLibStrikerWS ? "1" : "0"));
		PublicDefinitions.Add("WITH_WINHTTPWEBSOCKETS=" + (bWithWinHttpStrikerWS ? "1" : "0"));
		string PlatformInclude = WebSocketsManagerPlatformInclude;
		if (PlatformInclude.Length > 0)
		{
			PublicDefinitions.Add("WEBSOCKETS_MANAGER_PLATFORM_INCLUDE=\"" + WebSocketsManagerPlatformInclude + "\"");
			PublicDefinitions.Add("WEBSOCKETS_MANAGER_PLATFORM_CLASS=" + WebSocketsManagerPlatformClass);
		}
	}
}
