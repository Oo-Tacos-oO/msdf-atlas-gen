-- msdf-atlas-gen lib --

include "msdfgen"

project "msdf-atlas-gen"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
	systemversion "latest"

	targetdir (binariesDir .. "/%{prj.name}")
	objdir (binariesIntermediateDir .. "/%{prj.name}")

	files {
		"msdf-atlas-gen/**.h",
		"msdf-atlas-gen/**.hpp",
		"msdf-atlas-gen/**.cpp",
	}
	removefiles {
		"msdf-atlas-gen/**.vs*/**",
	}

	includedirs {
		"msdf-atlas-gen",
		"msdfgen",
		"msdfgen/include",
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS",
		"MSDF_ATLAS_PUBLIC=",

		--msdfgen
		"MSDFGEN_PUBLIC=",
	}

	links {
		"msdfgen",
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "speed"

	filter "configurations:Distribution"
		runtime "Release"
		optimize "speed"
		symbols "off"
