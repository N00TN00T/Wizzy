-- PREMAKE FOR LUA

project "lua"
    kind "StaticLib"
    language "C"
    
	targetdir ("bin/" .. output_dir .. "/%{prj.name}")
  objdir ("bin-int/" .. output_dir .. "/%{prj.name}")

	files
	{
		"src/**"
    }
  
	filter "system:windows"
    systemversion "latest"
    staticruntime "On"

  filter "system:linux"
    pic "On"
    systemversion "latest"
    staticruntime "On"

  filter "system:macosx"
    systemversion "latest"
    staticruntime "On"
