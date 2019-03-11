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
  imgui = "Wizzy/vendor/imgui"
}

-- Include the premake file from glfw directory
include "Wizzy/vendor/glfw/"
include "Wizzy/vendor/glad/"
include "Wizzy/vendor/imgui/"

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
    "%{include_dir.spdlog}",
    "%{include_dir.glfw}",
    "%{include_dir.glad}",
    "%{include_dir.imgui}"
  }

  links 
  {
    "glfw",
    "glad",
    "imgui"
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

    links
    {
      "opengl32.lib"
    }

  filter "system:linux"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"
    pic "On"
    defines "WZ_PLATFORM_LINUX"

    links
    {
      "Xrandr",
      "Xi",
      "X11"
    }

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

    filter { "system:windows", "configurations:Debug*" }
  buildoptions "/MDd"
    filter { "system:windows", "configurations:Release*" }
  buildoptions "/MD"
    filter { "system:windows", "configurations:Dist*" }
  buildoptions "/MD"

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
    "Wizzy/vendor/spdlog/include",
    "%{include_dir.imgui}"
  }

  links
  {
    "Wizzy"
  }
  
  filter "system:linux"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"
    pic "On"
    defines "WZ_PLATFORM_LINUX"

  filter "system:windows"
    cppdialect "gnu++17"
    staticruntime "On"
    systemversion "latest"

    defines 
    {
      "WZ_PLATFORM_WINDOWS"
    }

  filter "system:linux"
    systemversion "latest"
    defines "WZ_PLATFORM_LINUX"

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

  filter { "system:windows", "configurations:Debug*" }
    buildoptions "/MDd"
  filter { "system:windows", "configurations:Release*" }
    buildoptions "/MD"
  filter { "system:windows", "configurations:Dist*" }
    buildoptions "/MD"

--[[------------------------------------------------------------------------------------]]