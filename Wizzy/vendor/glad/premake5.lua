-- PREMAKE FOR GLAD

project "glad"
    kind "StaticLib"
    language "C"
    
	targetdir ("bin/" .. output_dir .. "/%{prj.name}")
  objdir ("bin-int/" .. output_dir .. "/%{prj.name}")

	files
	{
    "include/glad/glad.h",
    "include/KHR/khrplatform.h",
    "src/glad.c"
  }
  
  includedirs { "include" }
  
	filter "system:windows"
    systemversion "latest"
    staticruntime "On"

  filter { "system:windows", "configurations:Release" }
      buildoptions "/MT"

  filter "system:linux"
    pic "On"
    systemversion "latest"
    staticruntime "On"

  filter "system:macosx"
    systemversion "latest"
    staticruntime "On"

  filter "configurations:Debug*"
    defines "WZ_CONFIG_DEBUG"
    runtime "Debug"
    symbols "On"
    optimize "Off"

  filter "configurations:Release*"
    defines "WZ_CONFIG_RELEASE"
    runtime "Release"
    symbols "On"
    optimize "On"

  filter "configurations:Dist*"
    defines { "WZ_CONFIG_DIST", "WZ_DISABLE_ASSERTS" }
    runtime "Release"
    symbols "Off"
    optimize "Full"
