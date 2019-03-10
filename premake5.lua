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
  glfw = "Wizzy/vendor/glfw/include",
  glad = "Wizzy/vendor/glad/include"
}

-- Include the premake file from glfw directory
include "Wizzy/vendor/glfw/"
include "Wizzy/vendor/glad/"

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
    "%{include_dir.glfw}",
    "%{include_dir.glad}"
  }

  links 
  {
    "glfw",
    "glad",
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

  filter "configurations:Debug*"
    defines "WZ_CONFIG_DEBUG"
    buildoptions "/MDd"
    runtime "Debug"
    symbols "On"

  filter "configurations:Release*"
    defines "WZ_CONFIG_RELEASE"
    buildoptions "/MD"
    runtime "Release"
    optimize "On"

  filter "configurations:Dist*"
    defines 
    {
      "WZ_CONFIG_DIST",
      "WZ_DISABLE_ASSERTS"
    }
    buildoptions "/MD"
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

  filter "configurations:Debug*"
    defines "WZ_CONFIG_DEBUG"
    buildoptions "/MDd"
    runtime "Debug"
    symbols "On"

  filter "configurations:Release*"
    defines "WZ_CONFIG_RELEASE"
    buildoptions "/MD"
    runtime "Release"
    optimize "On"

  filter "configurations:Dist*"
    defines 
    {
      "WZ_CONFIG_DIST",
      "WZ_DISABLE_ASSERTS"
    }
    buildoptions "/MD"
    runtime "Release"
    optimize "On"

--[[------------------------------------------------------------------------------------]]