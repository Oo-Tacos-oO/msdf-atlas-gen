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
		"msdf-atlas-gen/**.cpp"
	}
	removefiles {
		"src/**.vs*/**"
	}

	includedirs {
		"msdf-atlas-gen",
		"msdfgen",
		"msdfgen/include",
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS",
	}

	links {
		"msdfgen",
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Distribution"
		runtime "Release"
		optimize "on"
		symbols "off"
