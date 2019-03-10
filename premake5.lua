-- PREMAKE FOR WIZZY

workspace "Wizzy"
  architecture "x64"
  
  configurations 
  {
    "Debug",
    "Release",
    "Distribution"
  }
  
output_dir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
  
-- Include directories relative to root directory
include_dir = {
  glfw = "Wizzy/vendor/glfw/include"
}

-- Include the premake file from glfw directory
include "Wizzy/vendor/glfw/"

--[[------------------------------------------------------------------------------------
       CORE PROJECT
----------------------------------------------------------------------------------------]]

project "Wizzy"
  location "Wizzy"
  kind "SharedLib"
  language "C++"
  staticruntime "off"
  
  targetdir ("bin/" .. output_dir .. "/%{prj.name}")
  objdir ("bin-int/" .. output_dir .. "/%{prj.name}")
  
  pchheader "wzpch.h"
  pchsource "Wizzy/src/wzpch.cpp"

  files 
  {
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp"
  }
  
  includedirs
  {
    "%{prj.name}/src",
    "%{prj.name}/vendor/spdlog/include",
    "%{include_dir.glfw}"
  }

  links 
  {
    "glfw",
    "opengl32.lib"
  }

  defines "WZ_EXPORT"

  filter "system:windows"
    cppdialect "gnu++17"
    staticruntime "On"
    systemversion "latest"

    defines 
    {
      "WZ_PLATFORM_WINDOWS",
      "GLFW_INCLUDE_NONE"
    }

    postbuildcommands
    {
      ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. output_dir .. "/Sandbox")
    }

  filter "configurations:Debug"
    defines 
    {
      "WZ_CONFIG_DEBUG"
    }
    runtime "Debug"
    symbols "On"

  filter "configurations:Release"
    defines 
    {
      "WZ_CONFIG_RELEASE"
    }
    runtime "Release"
    optimize "On"

  filter "configurations:Distribution"
    defines 
    {
      "WZ_CONFIG_DISTRIBUTION",
      "WZ_DISABLE_ASSERTS"
    }
    runtime "Release"
    optimize "On"

  --filter { "system:windows", "configurations:Release" }
    --buildoptions "/MT"
  --filter { "system:windows", "configurations:Distribution" }
    --buildoptions "/MT"

--[[------------------------------------------------------------------------------------]]


--[[------------------------------------------------------------------------------------
       SANDBOX PROJECT
----------------------------------------------------------------------------------------]]
project "Sandbox"
  location "Sandbox"
  kind "ConsoleApp"
  language "C++"
  architecture "x64"
  staticruntime "off"
  
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
    "Wizzy/vendor/spdlog/include"
  }

  links
  {
    "Wizzy"
  }
  
  filter "system:windows"
    cppdialect "gnu++17"
    staticruntime "On"
    systemversion "latest"

    defines 
    {
      "WZ_PLATFORM_WINDOWS"
    }

  filter "configurations:Debug"
    defines 
    {
      "WZ_CONFIG_DEBUG"
    }
    runtime "Debug"
    symbols "On"

  filter "configurations:Release"
    defines 
    {
      "WZ_CONFIG_RELEASE"
    }
    runtime "Release"
    optimize "On"

  filter "configurations:Distribution"
    defines 
    {
      "WZ_CONFIG_DISTRIBUTION",
      "WZ_DISABLE_ASSERTS"
    }
    runtime "Release"
    optimize "On"

  --[[filter { "system:windows", "configurations:Release" }
    buildoptions "/MT"
  filter { "system:windows", "configurations:Distribution" }
    buildoptions "/MT"]]

--[[------------------------------------------------------------------------------------]]