-- Helpful premake documentation
-- Tokens https://github.com/premake/premake-core/wiki/Tokens

workspace ("Insight")
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Engine-Dist",
		"Game-Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

gameName = "Application"

IncludeDir = {}
IncludeDir["ImGui"] = "Engine/Vendor/imgui"
IncludeDir["assimp"] = "Engine/Vendor/assimp-3.3.1/include"
IncludeDir["DX12TK"] = "Engine/Vendor/Microsoft/DirectX12/TK/Inc"
IncludeDir["ImGuizmo"] = "Engine/Vendor/ImGuizmo"
IncludeDir["rapidjson"] = "Engine/Vendor/rapidjson"
IncludeDir["spdlog"] = "Engine/Vendor/spdlog"
IncludeDir["Mono"] = "Engine/Vendor/Mono/include/mono-2.0"

include "Engine/Vendor/ImGui"

CustomDefines = {}
CustomDefines["IE_BUILD_DIR"] = "../Bin" .. outputdir

project ("Engine")
	location ("Engine")
	kind "WindowedApp"
	-- kind "ConsoleApp"
	--[[
	TODO:   Change kind to ConsoleApp when using Vulkan
			or change kind to WindowedApp when using DX12
	--]]
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Bin-Int/" .. outputdir .. "/%{prj.name}")

	pchheader "ie_pch.h"
	pchsource "Engine/Source/ie_pch.cpp" 

	files
	{
		"%{prj.name}/Vendor/Vendor_Build.cpp",
		"%{prj.name}/Source/**.cpp",
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.hlsl",
		"%{prj.name}/Source/**.hlsli",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"IE_BUILD_DIR=%{CustomDefines.IE_BUILD_DIR}/Engine/"
	}

	includedirs
	{
		"%{prj.name}/Vendor/Microsoft/DirectX12",
		"%{prj.name}/Vendor/Nvidia/DirectX12",
		"%{IncludeDir.rapidjson}/include/",
		"%{IncludeDir.spdlog}/include/",
		"%{IncludeDir.ImGuizmo}/",
		"%{IncludeDir.Mono}/",
		"%{IncludeDir.DX12TK}/",
		"%{IncludeDir.ImGui}/",
		"%{IncludeDir.assimp}/",
		"%{prj.name}/Source/",
		gameName .. "/Source/"
	}

	links
	{
		"d3d12.lib",
		"dxgi.lib",
		"d3dcompiler.lib",
		"DirectXTK12.lib",
		"assimp-vc140-mt.lib",
		"MonoPosixHelper.lib",
		"mono-2.0-sgen.lib",
		"libmono-static-sgen.lib",
		"ImGui",
	}

	libdirs
	{
		"Engine/Vendor/assimp-3.3.1/build/code/%{cfg.buildcfg}",
		"Engine/Vendor/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/%{cfg.buildcfg}",
		"Engine/Vendor/Mono/lib",
	}

	postbuildcommands
	{
		("{COPY} %{wks.location}Engine/Vendor/assimp-3.3.1/build/code/%{cfg.buildcfg}/assimp-vc140-mt.dll ../bin/"..outputdir.."/Engine"),
		("{COPY} %{wks.location}Engine/Vendor/Mono/bin/mono-2.0-sgen.dll ../bin/"..outputdir.."/Engine")
	}



	filter "system:windows"
		systemversion "latest"

		defines
		{
			"IE_PLATFORM_WINDOWS"
		}

		flags
		{
			"MultiProcessorCompile"
		}

		filter { "files:**.hlsl" }
			shadermodel "5.0"

		filter { "files:**.pixel.hlsl" }
			shadertype "Pixel"
		
		filter { "files:**.vertex.hlsl" }
			shadertype "Vertex"
	
	-- Engine Development
	filter "configurations:Debug"
		defines "IE_DEBUG"
		runtime "Debug"
		symbols "on"
	
	-- Engine Release
	filter "configurations:Release"
		defines "IE_RELEASE"
		runtime "Release"
		optimize "on"
		symbols "on"
	
	-- Full Engine Distribution, all performance logs and debugging windows stripped
	filter "configurations:Engine-Dist"
		defines "IE_ENGINE_DIST"
		runtime "Release"
		optimize "on"
		symbols "on"

	-- Full Game Distribution, all engine debug tools(leel editors, editor user interfaces) stripped
	filter "configurations:Game-Dist"
		defines "IE_GAME_DIST"
		runtime "Release"
		optimize "on"
		symbols "on"

project (gameName)
	location (gameName)
	kind "StaticLib"
	cppdialect "C++17"
	language "C++"
	staticruntime "off"

	targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Bin-Int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp",
	}

	includedirs
	{
		"Engine/Vendor/assimp-3.3.1/include",
		"Engine/Vendor/Microsoft/DirectX12/TK/Inc",
		"Engine/Vendor/Microsoft/DirectX12",
		"Engine/Vendor/Nvidia/DirectX12",
		"Engine/Vendor/spdlog/include",
		"Engine/Vendor/rapidjson/include",
		"Engine/Vendor/Mono/include/mono-2.0",
		"Engine/Source",
		"Engine/Vendor"
	}

	links
	{
		"Engine"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"IE_PLATFORM_WINDOWS",
			"IE_BUILD_DLL"
		}

	filter "configurations:Debug"
		defines "IE_DEBUG"
		symbols "on"
	
	filter "configurations:Release"
		defines "IE_RELEASE"
		symbols "on"
		optimize "on"

	filter "configurations:EngineDist"
		defines "IE_ENGINE_DIST"
		optimize "on"
		symbols "on"
		
	filter "configurations:GameDist"
		defines "IE_GAME_DIST"
		optimize "on"
		symbols "on"


project ("Assembly-CSharp")
	location("Assembly-CSharp")
	kind("SharedLib")
	language("C#")
	targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Bin-Int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/Source/**.cs",
		"%{prj.name}/Source/**.xaml",
		"%{prj.name}/Source/**.xaml.cs",
		"%{prj.name}/Source/**.config"
	}

	links
	{
		"Microsoft.CSharp",
		"PresentationCore",
		"PresentationFramework",
		"System",
		"System.Core",
		"System.Data",
		"System.Data.DataSetExtensions",
		"System.Net.Http",
		"System.Xaml",
		"System.Xml",
		"System.Xml.Linq",
		"WindowsBase",
	}

	postbuildcommands
	{
		("{COPY} %{wks.location}bin/"..outputdir.."/Assembly-CSharp/Assembly-CSharp.dll ../bin/"..outputdir.."/Engine")
	}