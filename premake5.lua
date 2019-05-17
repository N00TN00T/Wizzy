-- PREMAKE FOR WIZZY

workspace "Wizzy"

  configurations
  {
    "Debug64",
    "Release64",
    "Dist64",
    "DebugARM",
    "ReleaseARM",
    "DistARM"
  }

  filter "configurations:*64"
    architecture "x64"

filter "configurations:*ARM"
    architecture "ARM"

output_dir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root directory
include_dir = {
  spdlog = "Wizzy/vendor/spdlog/include",
  glfw = "Wizzy/vendor/glfw/include",
  glad = "Wizzy/vendor/glad/include",
  imgui = "Wizzy/vendor/imgui",
  glm = "Wizzy/vendor/glm",
  stb = "Wizzy/vendor/stb",
  rttr = "Wizzy/vendor/rttr"
}

include "Wizzy/vendor/glfw/"
include "Wizzy/vendor/glad/"
include "Wizzy/vendor/imgui/"
include "Wizzy/vendor/rttr"

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

  files
  {
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp"
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
    "%{include_dir.rttr}"
  }

  defines {"WZ_EXPORT", "WZ_USE_OPENGL", "STB_IMAGE_IMPLEMENTATION"}

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
      "GLFW_INCLUDE_NONE"
    }

    links "opengl32.lib"

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
    defines "WZ_PLATFORM_LINUX"

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
    optimize "On"

  filter "configurations:Dist*"
    defines
    {
      "WZ_CONFIG_DIST",
      "WZ_DISABLE_ASSERTS"
    }
    runtime "Release"
    optimize "On"

--[[------------------------------------------------------------------------------------]]


--[[------------------------------------------------------------------------------------
       SANDBOX PROJECT
----------------------------------------------------------------------------------------]]
project "Sandbox"
  location "Sandbox"
  kind "ConsoleApp"
  language "C++"
  architecture "x64"

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
    "Wizzy/vendor/spdlog/include",
    "%{include_dir.imgui}",
    "%{include_dir.glm}",
    "%{include_dir.spdlog}",
    "%{include_dir.glfw}",
    "%{include_dir.glad}",
    "%{include_dir.glm}",
    "%{include_dir.stb}",
    "%{include_dir.rttr}"
  }

  links
  {
    "Wizzy",
    "imgui",
    "glfw",
    "glad"
  }

  defines { "WZ_USE_OPENGL", "STB_IMAGE_IMPLEMENTATION" }

---------------------------------------------------------------------
--                        LINUX
---------------------------------------------------------------------
  filter "system:linux"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"

    pic "On"

    defines "WZ_PLATFORM_LINUX"

    links
    {
      "GL",
      "GLU",
      "X11",
      "Xxf86vm",
      "Xrandr",
      "pthread",
      "Xi",
      "dl"
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
      "GLFW_INCLUDE_NONE"
    }

    -- Windows-specific files
    files
    {
      "%{prj.name}/src/Wizzy/Platform/windows/**.h",
      "%{prj.name}/src/Wizzy/Platform/windows/**.cpp"
    }

    links { "opengl32.lib", "glu32.lib" }

----------------------------------------------------------------------

---------------------------------------------------------------------
--                        MACOSX
---------------------------------------------------------------------
  filter "system:macosx"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"

    defines "WZ_PLATFORM_MACOSX"

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
    optimize "On"

  filter "configurations:Dist*"
    defines
    {
      "WZ_CONFIG_DIST",
      "WZ_DISABLE_ASSERTS"
    }
    runtime "Release"
    optimize "On"

--[[------------------------------------------------------------------------------------]]
