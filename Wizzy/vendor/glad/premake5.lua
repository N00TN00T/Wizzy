-- PREMAKE FOR GLAD

project "glad"
    kind "StaticLib"
    language "C"
    systemversion "latest"
    staticruntime "on"

    targetdir ("bin/" .. output_dir .. "/%{prj.name}")
    objdir ("bin-int/" .. output_dir .. "/%{prj.name}")

    files
    {
        "include/glad/glad.h",
        "include/KHR/khrplatform.h",
        "src/glad.c"
    }

    includedirs { "include" }

    filter { "system:windows", "configurations:Release" }
        buildoptions "/MT"

    filter "system:linux"
        pic "On"

    filter "system:macosx"
        pic "on"

    filter "configurations:Debug*"
        runtime "Debug"
        symbols "on"
        optimize "off"

    filter "configurations:Release*"
        runtime "Release"
        symbols "off"
        optimize "on"

    filter "configurations:Dist*"
        runtime "release"
        symbols "off"
        optimize "full"
