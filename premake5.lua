workspace "Wizzy"
  architecture "x64"
  
  configurations 
  {
    "Debug",
    "Release",
    "Distribution"
  }
  
output_dir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
  
--[[------------------------------------------------------------------------------------
       CORE PROJECT
----------------------------------------------------------------------------------------]]

project "Wizzy"
  location "Wizzy"
  kind "SharedLib"
  language "C++"
  
  targetdir ("bin/" .. output_dir .. "/%{prj.name}")
  objdir ("bin-int/" .. output_dir .. "/%{prj.name}")
  
  files 
  {
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp"
  }
  
  includedirs
  {
    "%{prj.name}/vendor/spdlog/include"
  }

  filter "system:windows"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"

    defines 
    {
      "WZ_PLATFORM_WINDOWS",
      "WZ_BUILD_DLL"
    }

    postbuildcommands
    {
      --"copy %{cfg.buildtarget.relpath} bin/" .. output_dir .. "/Sandbox"
      ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. output_dir .. "/Sandbox")
    }

  filter "configurations:Debug"
    defines 
    {
      "WZ_CONFIG_DEBUG"
    }
    symbols "On"

  filter "configurations:Release"
    defines 
    {
      "WZ_CONFIG_RELEASE"
    }
    optimize "On"

  filter "configurations:Distribution"
    defines 
    {
      "WZ_CONFIG_DISTRIBUTION"
    }
    optimize "On"

  filter { "system:windows", "configurations:Release" }
    buildoptions "/MT"
  filter { "system:windows", "configurations:Distribution" }
    buildoptions "/MT"

--[[------------------------------------------------------------------------------------]]


--[[------------------------------------------------------------------------------------
       SANDBOX PROJECT
----------------------------------------------------------------------------------------]]
project "Sandbox"
  location "Sandbox"
  kind "ConsoleApp"
  language "C++"
  
  targetdir ("bin/" .. output_dir .. "/%{prj.name}")
  objdir ("bin-int/" .. output_dir .. "/%{prj.name}")
  
  files 
  {
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp"
  }
  
  includedirs
  {
    "Wizzy/src",
    "Wizzy/vendor/spdlog/include"
  }

  links
  {
    "Wizzy"
  }
  
  filter "system:windows"
    cppdialect "C++17"
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
    symbols "On"

  filter "configurations:Release"
    defines 
    {
      "WZ_CONFIG_RELEASE"
    }
    optimize "On"

  filter "configurations:Distribution"
    defines 
    {
      "WZ_CONFIG_DISTRIBUTION"
    }
    optimize "On"

  --[[filter { "system:windows", "configurations:Release" }
    buildoptions "/MT"
  filter { "system:windows", "configurations:Distribution" }
    buildoptions "/MT"]]

--[[------------------------------------------------------------------------------------]]