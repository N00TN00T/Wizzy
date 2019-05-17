-- PREMAKE FOR RTTR

project "rttr"
    kind "StaticLib"
    language "C++"

    targetdir ("bin/" .. output_dir .. "/%{prj.name}")
    objdir ("bin-int/" .. output_dir .. "/%{prj.name}")

	files
    {
        "src/**"
    }

    includedirs
    {
        "src/",
        "deps/"
    }

	filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "on"

    filter "system:linux"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "on"
        pic "on"

    filter "system:macosx"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "on"

    filter "configurations:Debug*"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release*"
        runtime "Release"
        symbols "on"
        optimize "on"

    filter "configurations:Dist*"
        runtime "Release"
        optimize "on"
