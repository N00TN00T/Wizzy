-- PREMAKE FOR WIZZY

workspace "Wizzy"

  startproject "Launcher"

  architecture "x64"

  configurations
  {
    "Debug64",
    "Release64",
    "Dist64",
  }

  files { "**premake5.lua" }

  output_dir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

  -- Include directories relative to root directory
  include_dir = {
    spdlog = "Wizzy/vendor/spdlog/include",
    glfw = "Wizzy/vendor/glfw/include",
    glad = "Wizzy/vendor/glad/include",
    imgui = "Wizzy/vendor/imgui",
    glm = "Wizzy/vendor/glm",
    stb = "Wizzy/vendor/stb",
    lua = "Wizzy/vendor/lua",
    pybind11 = "Wizzy/vendor/pybind11/include",
    pybind11 = "Wizzy/vendor/ulib/include",
    pdoes = "Wizzy/vendor/pdoes"
  }

  include "Wizzy/vendor/glfw/"
  include "Wizzy/vendor/glad/"
  include "Wizzy/vendor/imgui/"
  include "Wizzy/vendor/lua"
  include "Wizzy/vendor/pdoes"

--[[------------------------------------------------------------------------------------
       LAUNCHER PROJECT
----------------------------------------------------------------------------------------]]

project "Launcher"
	location "Launcher"
	kind "ConsoleApp"
  language "C++"

	targetdir ("bin/" .. output_dir .. "/%{prj.name}/")
  objdir ("bin-int/" .. output_dir .. "/%{prj.name}/")

  buildoptions { "-Wall" }

  files
  {
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp",
  }

  includedirs
  {
		"Wizzy/src",
    "%{prj.name}/src",
    "%{include_dir.spdlog}",
    "%{include_dir.glfw}",
    "%{include_dir.glad}",
    "%{include_dir.imgui}",
    "%{include_dir.glm}",
    "%{include_dir.stb}",
    "%{include_dir.lua}",
    "%{include_dir.pybind11}",
    "%{include_dir.ulib}",
    "%{include_dir.pdoes}"
  }

	defines
  {
      "WZ_USE_OPENGL",
      "STB_IMAGE_IMPLEMENTATION",
      "ULIB_NO_INCLUDE"
  }

	links
  {
	"Sandbox",
    "Wizzy",
    "imgui",
    "glfw",
    "glad",
    "lua",
    "pdoes"
  }

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
      "PDOES_API=__declspec(dllimport)"
    }

    links { "opengl32.lib", "glu32.lib" }

---------------------------------------------------------------------


---------------------------------------------------------------------
--                        LINUX
---------------------------------------------------------------------
  filter "system:linux"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"
    pic "On"
    defines {
        "WZ_PLATFORM_LINUX"
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
      "stdc++fs"
    }

    includedirs { "/usr/include/python3.6" }



---------------------------------------------------------------------

---------------------------------------------------------------------
--                        MACOSX
---------------------------------------------------------------------
  filter "system:macosx"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"
    pic "On"
    defines {
        "WZ_PLATFORM_MACOSX"
    }
---------------------------------------------------------------------

  filter "configurations:Debug*"
    defines "WZ_CONFIG_DEBUG"
    runtime "Debug"
    symbols "On"

  filter "configurations:Release*"
    defines "WZ_CONFIG_RELEASE"
    runtime "Release"
    symbols "On"
    optimize "On"

  filter "configurations:Dist*"
    defines { "WZ_CONFIG_DIST", "WZ_DISABLE_ASSERTS" }
    runtime "Release"
    optimize "On"

  filter "action:codelite"
    defines { "__CODELITE__" }

  filter "action:xcode4"
    defines { "__XCODE__" }

--[[------------------------------------------------------------------------------------
       CORE PROJECT
----------------------------------------------------------------------------------------]]

project "Wizzy"
  location "Wizzy"
  kind "StaticLib"
  language "C++"

  targetdir ("bin/" .. output_dir .. "/%{prj.name}")
  objdir ("bin-int/" .. output_dir .. "/%{prj.name}")

  pchheader "wzpch.h"
  pchsource "Wizzy/src/wzpch.cpp"

  buildoptions { "-Wall" }

  files
  {
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp",
  }

  -- Exclude all Platform-specific ones to later include the ones the target Platform will use
  removefiles
  {
    "%{prj.name}/src/Wizzy/Platform/**"
  }

  includedirs
  {
    "%{prj.name}/src",
    "%{include_dir.spdlog}",
    "%{include_dir.glfw}",
    "%{include_dir.glad}",
    "%{include_dir.imgui}",
    "%{include_dir.glm}",
    "%{include_dir.stb}",
    "%{include_dir.lua}",
    "%{include_dir.pybind11}",
    "%{include_dir.ulib}",
    "%{include_dir.pdoes}"
  }

  defines
  {
    "WZ_EXPORT",
    "WZ_USE_OPENGL",
    "STB_IMAGE_IMPLEMENTATION"
  }

  filter "action:codelite"
    defines { "__CODELITE__" }

  filter "action:xcode4"
    defines { "__XCODE__" }

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
      "PDOES_API=__declspec(dllimport)"
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
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"
    pic "On"

    defines {
        "WZ_PLATFORM_LINUX",
        "PDOES_API=__attribute__((visibility(\"default\")))"
    }

    -- Linux-specific files
    files
    {
      "%{prj.name}/src/Wizzy/platform/linux/**.h",
      "%{prj.name}/src/Wizzy/platform/linux/**.cpp",
      "%{prj.name}/src/Wizzy/platform/OpenGL/**.h",
      "%{prj.name}/src/Wizzy/platform/OpenGL/**.cpp"
    }

    includedirs { "/usr/include/python3.6" }



---------------------------------------------------------------------

---------------------------------------------------------------------
--                        MACOSX
---------------------------------------------------------------------
  filter "system:macosx"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"
    pic "On"
    defines "WZ_PLATFORM_LINUX"

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
    symbols "On"

  filter "configurations:Release*"
    defines "WZ_CONFIG_RELEASE"
    runtime "Release"
    symbols "On"
    optimize "On"

  filter "configurations:Dist*"
    defines { "WZ_CONFIG_DIST", "WZ_DISABLE_ASSERTS" }
    runtime "Release"
    optimize "On"

--[[------------------------------------------------------------------------------------]]


--[[------------------------------------------------------------------------------------
       SANDBOX PROJECT
----------------------------------------------------------------------------------------]]
project "Sandbox"
  location "Sandbox"
  kind "SharedLib"
  language "C++"
  architecture "x64"

  targetdir ("bin/" .. output_dir .. "/%{prj.name}")
  objdir ("bin-int/" .. output_dir .. "/%{prj.name}")

  pchheader "spch.h"
  pchsource "Sandbox/src/spch.cpp"

  buildoptions { "-Wall" }

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
    "%{include_dir.glm}",
    "%{include_dir.spdlog}",
    "%{include_dir.glfw}",
    "%{include_dir.glad}",
    "%{include_dir.glm}",
    "%{include_dir.stb}",
    "%{include_dir.lua}",
    "%{include_dir.pybind11}",
    "%{include_dir.ulib}",
    "%{include_dir.pdoes}"
  }

  defines
  {
      "WZ_USE_OPENGL",
      "STB_IMAGE_IMPLEMENTATION",
      "ULIB_NO_INCLUDE",
      "WZ_EXPORT"
  }

  links
  {
	"Wizzy",
    "imgui",
    "glfw",
    "glad",
    "lua",
    "pdoes"
  }

  postbuildcommands { ("cp %{cfg.buildtarget.relpath} ../bin/" .. output_dir .. "/Launcher/SandboxModule") }

  filter "action:codelite"
    defines { "__CODELITE__" }

  filter "action:xcode4"
    defines { "__XCODE__" }

---------------------------------------------------------------------
--                        LINUX
---------------------------------------------------------------------
  filter "system:linux"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"

    pic "On"

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
      "stdc++fs"
    }

    includedirs { "/usr/include/python3.6" }
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
      "PDOES_API=__declspec(dllexport)"
    }

		links { "opengl32.lib", "glu32.lib" }

    -- Windows-specific files
    files
    {
      "%{prj.name}/src/Wizzy/Platform/windows/**.h",
      "%{prj.name}/src/Wizzy/Platform/windows/**.cpp"
    }

----------------------------------------------------------------------

---------------------------------------------------------------------
--                        MACOSX
---------------------------------------------------------------------
  filter "system:macosx"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"

    defines {
        "WZ_PLATFORM_MACOSX",
        "PDOES_API=__attribute__((visibility(\"default\")))"
    }

    -- Macosx-specific files
    files
    {
      "%{prj.name}/src/Wizzy/Platform/macosx/**.h",
      "%{prj.name}/src/Wizzy/Platform/macosx/**.cpp"
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
    symbols "On"

  filter "configurations:Release*"
    defines "WZ_CONFIG_RELEASE"
    runtime "Release"
    symbols "On"
    optimize "On"

  filter "configurations:Dist*"
    defines { "WZ_CONFIG_DIST", "WZ_DISABLE_ASSERTS" }
    runtime "Release"
    optimize "On"

--[[------------------------------------------------------------------------------------]]
