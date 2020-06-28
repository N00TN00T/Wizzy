-- PREMAKE FOR WIZZY

workspace "Wizzy"

    startproject "Sandbox"

    architecture "x64"

    configurations
    {
        "Debug64",
        "Release64",
        "Dist64",
    }

    flags {
        "MultiProcessorCompile"
    }

    files { "**premake5.lua" }

    output_dir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    -- Include directories relative to root directory
    include_dir = {
        spdlog = "Wizzy/vendor/spdlog/include",
        glfw = "Wizzy/vendor/glfw/include",
        glad = "Wizzy/vendor/glad/include",
        imgui = "Wizzy/vendor/imgui",
        stb = "Wizzy/vendor/stb",
        ulib = "Wizzy/vendor/ulib/include",
        assimp = "Wizzy/vendor/assimp/include",
        pfd = "Wizzy/vendor/pfd",
        ft = "Wizzy/vendor/freetype/windows/include",
        box2d = "Wizzy/vendor/box2d/include"
    }

    include "Wizzy/vendor/glfw/"
    include "Wizzy/vendor/glad/"
    include "Wizzy/vendor/imgui/"
    include "Wizzy/vendor/box2d"

--[[------------------------------------------------------------------------------------
        CORE PROJECT
----------------------------------------------------------------------------------------]]

    project "Wizzy"
        location "Wizzy"
        kind "StaticLib"
        language "C++"
        cppdialect "C++17"
        staticruntime "on"
        systemversion "latest"

        targetdir ("bin/" .. output_dir .. "/%{prj.name}")
        objdir ("bin-int/" .. output_dir .. "/%{prj.name}")

        pchheader "wzpch.h"
        pchsource "Wizzy/src/wzpch.cpp"

        files
        {
            "%{prj.name}/src/**.h",
            "%{prj.name}/src/**.cpp",
        }

        -- Exclude all Platform-specific ones to later include the ones the target Platform will use
        removefiles
        {
            "%{prj.name}/src/Wizzy/Platform/**",
            "%{prj.name}/src/Wizzy/Scripting/**",
        }

        includedirs
        {
            "%{prj.name}/src",
            "%{include_dir.spdlog}",
            "%{include_dir.glfw}",
            "%{include_dir.glad}",
            "%{include_dir.imgui}",
            "%{include_dir.stb}",
            "%{include_dir.ulib}",
            "%{include_dir.assimp}",
            "%{include_dir.pfd}",
            "%{include_dir.ft}",
            "%{include_dir.box2d}"
        }

        defines
        {
            "WZ_EXPORT",
            "WZ_USE_OPENGL",
            "FREETYPE_STATIC",
            "WZ_COMPILE_EXTENSIONS"
        }

        filter "action:codelite"
            defines { "__CODELITE__" }

        filter "action:xcode4"
            defines { "__XCODE__" }

---------------------------------------------------------------------
--                        WINDOWS
---------------------------------------------------------------------
        filter "system:windows"
            defines
            {
                "WZ_PLATFORM_WINDOWS",
            }

            -- Windows-specific files
            files
            {
                "%{prj.name}/src/Wizzy/platform/windows/**.h",
                "%{prj.name}/src/Wizzy/platform/windows/**.cpp",
                "%{prj.name}/src/Wizzy/platform/OpenGL/**.h",
                "%{prj.name}/src/Wizzy/platform/OpenGL/**.cpp"
            }
---------------------------------------------------------------------

---------------------------------------------------------------------
--                        LINUX
---------------------------------------------------------------------
        filter "system:linux"
            pic "on"

            defines {
                "WZ_PLATFORM_LINUX"
            }

            -- Linux-specific files
            files
            {
                "%{prj.name}/src/Wizzy/platform/linux/**.h",
                "%{prj.name}/src/Wizzy/platform/linux/**.cpp",
                "%{prj.name}/src/Wizzy/platform/OpenGL/**.h",
                "%{prj.name}/src/Wizzy/platform/OpenGL/**.cpp"
            }

---------------------------------------------------------------------

---------------------------------------------------------------------
--                        MACOSX
---------------------------------------------------------------------
        filter "system:macosx"
            pic "on"
            defines "WZ_PLATFORM_MACOS"

            -- Macosx-specific files
            files
            {
                "%{prj.name}/src/Wizzy/platform/macosx/**.h",
                "%{prj.name}/src/Wizzy/platform/macosx/**.cpp"
            }
---------------------------------------------------------------------

        filter "configurations:Debug*"
            defines "WZ_CONFIG_DEBUG"
            runtime "Debug"
            symbols "on"
            optimize "off"

        filter "configurations:Release*"
            defines "WZ_CONFIG_RELEASE"
            runtime "Release"
            symbols "off"
            optimize "on"

        filter "configurations:Dist*"
            defines { "WZ_CONFIG_DIST", "WZ_DISABLE_ASSERTS" }
            runtime "release"
            symbols "off"
            optimize "full"

--[[------------------------------------------------------------------------------------]]


--[[------------------------------------------------------------------------------------
        SANDBOX PROJECT
----------------------------------------------------------------------------------------]]
    project "Sandbox"
        location "Sandbox"
        kind "ConsoleApp"
        language "C++"
        architecture "x64"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        targetdir ("bin/" .. output_dir .. "/%{prj.name}")
        objdir ("bin-int/" .. output_dir .. "/%{prj.name}")

        pchheader "spch.h"
        pchsource "Sandbox/src/spch.cpp"

        files
        {
            "%{prj.name}/src/**.h",
            "%{prj.name}/src/**.cpp"
        }

        includedirs
        {
            "Wizzy/src",
            "Sandbox/src",
            "%{include_dir.imgui}",
            "%{include_dir.spdlog}",
            "%{include_dir.glfw}",
            "%{include_dir.glad}",
            "%{include_dir.stb}",
            "%{include_dir.ulib}",
            "%{include_dir.assimp}",
            "%{include_dir.pfd}",
            "%{include_dir.ft}",
            "%{include_dir.box2d}"
        }

        defines
        {
            "WZ_USE_OPENGL",
            "ULIB_NO_INCLUDE",
            "WZ_EXPORT",
            "FREETYPE_STATIC",
            "WZ_EXTENSIONS_ENABLE_ALL"
        }

        links
        {
            "Wizzy",
            "imgui",
            "glfw",
            "glad",
            "box2d"
        }

        filter "action:codelite"
            defines { "__CODELITE__" }

        filter "action:xcode4"
            defines { "__XCODE__" }

---------------------------------------------------------------------
--                        LINUX
---------------------------------------------------------------------
        filter "system:linux"
            pic "on"

            defines {
                "WZ_PLATFORM_LINUX",
                "PDOES_API=__attribute__((visibility(\"default\")))"
            }

            links
            {
                "GL",
                "GLU",
                "X11",
                "Xxf86vm",
                "Xrandr",
                "pthread",
                "Xi",
                "dl",
                "stdc++fs",
                "Wizzy/vendor/assimp/linux/lib/assimp-vc140-mt"
            }

---------------------------------------------------------------------

---------------------------------------------------------------------
--                        WINDOWS
---------------------------------------------------------------------
        filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "WZ_PLATFORM_WINDOWS",
            "GLFW_INCLUDE_NONE",
        }

        links
        {
            "opengl32.lib",
            "glu32.lib",
            "Wizzy/vendor/assimp/windows/lib/assimp-vc140-mt",
            "%{wks.location}/Wizzy/vendor/freetype/windows/lib/freetype.lib"
        }

        -- Windows-specific files
        files
        {
            "%{prj.name}/src/Wizzy/platform/windows/**.h",
            "%{prj.name}/src/Wizzy/platform/windows/**.cpp"
        }

----------------------------------------------------------------------

---------------------------------------------------------------------
--                        MACOSX
---------------------------------------------------------------------
        filter "system:macosx"
            pic "on"

            defines {
                "WZ_PLATFORM_MACOSX",
                "PDOES_API=__attribute__((visibility(\"default\")))"
            }

            -- Macosx-specific files
            files
            {
                "%{prj.name}/src/Wizzy/platform/macosx/**.h",
                "%{prj.name}/src/Wizzy/platform/macosx/**.cpp"
            }

            links
            {
                "X11",
                "Xxf86vm",
                "Xrandr",
                "pthread",
                "Xi",
                "dl"
            }
---------------------------------------------------------------------

        filter "configurations:Debug*"
            defines "WZ_CONFIG_DEBUG"
            runtime "Debug"
            symbols "on"
            optimize "off"

        filter "configurations:Release*"
            defines "WZ_CONFIG_RELEASE"
            runtime "Release"
            symbols "off"
            optimize "on"

        filter "configurations:Dist*"
            defines { "WZ_CONFIG_DIST", "WZ_DISABLE_ASSERTS" }
            runtime "release"
            symbols "off"
            optimize "full"

--[[------------------------------------------------------------------------------------]]
