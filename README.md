# Wizzy Engine
Wizzy engine aims to be a cross-platform interactive application engine for creating any form of interactive applications from a 3D modelling application or a 2D graphics application to a game engine.

## Building
Wizzy Engine is currently not as smooth as it could be to use as a dependency for your project, but it's definitely possible. In its current state, it's recommended that you simply create a client project to be part of the Wizzy workspace. This will let you easily link to all dependencies and Wizzy while also being able to pull updates and rebuild them without issues. It would look like something like this:
`git clone https://github.com/N00TN00T/Wizzy`
`cd Wizzy`
`git submodule update --init`
`cd ..`
`mkdir MyProject`
Then, in MyProject directory, create a pch.h, pch.cpp and MyProject.cpp. Make sure that pch.cpp includes `#include "pch.h"` and that pch.h includes `#include <wzpch.h>`. 
Here is some example code for MyProject.cpp that simply renders an image of mario and shows how to set things up:
`
#include "pch.h"
#include <Wizzy.h>

struct ExampleComponent : public wz::Component<ExampleComponent>
{
	wz::Texture::Handle hMarioTexture; // The handle to be associated with the mario texture in ResourceManagement
	vec2 position = vec2(0, 0); // The position to render the mario texture at
	wz::Shader::Handle hShader; // The shader to be used in rendering
	mat4 projection; // The view projection to be used for rendering
};

class ExampleSystem : public wz::System
{
public:
	ExampleSystem()
	{
		AddComponentType<ExampleComponent>();

		Subscribe(wz::EventType::app_init);
		Subscribe(wz::EventType::app_update);
		Subscribe(wz::EventType::app_render);
		Subscribe(wz::EventType::window_resize);
	}

	virtual void OnEvent(const wz::Event& e, wz::ComponentGroup& components) const override
	{
		ExampleComponent* component = components.Get<ExampleComponent>();

		switch (e.GetEventType())
		{
			case wz::EventType::app_init:
			{
				WZ_DEBUG("Init event!");

				// Set the root resource directory
				wz::ResourceManagement::SetResourceDir("../res/");

				// IMPORTANT!! Always save handles for resources. If you lose a handle,
				// the only way to access the resource is by knowing it's exact res path/name.
				// Think of resource handles as C++ pointers.
				// Load mario.png from root resource directory ("../res/mario.png")
				component->hMarioTexture = wz::ResourceManagement::Load<wz::Texture>("mario.png");

				// Load the texture shader
				component->hShader = wz::ResourceManagement::Load<wz::Shader>("texture2d.shader");

				// Handles can be redeemed for a reference to the resource. It's recommended
				// that you don't save any form of resource references, and only use the 
				// resource handles.
				// Retrieve the texture associated with the handle, and print some properties
				wz::Texture& marioTexture = wz::ResourceManagement::Get<wz::Texture>(component->hMarioTexture);
				WZ_DEBUG("Texture width: {0}, height: {1}", marioTexture.GetWidth(), marioTexture.GetHeight());

				// Get a reference to the application window
				auto& window = wz::Application::Get().GetWindow();
				// Set the view projection to a normal 2D projection aligned with the window
				component->projection = glm::ortho<float>(0, window.GetWidth(), 0, window.GetHeight());

				// Set the clear background color to blue
				wz::RenderCommand::SetClearColor(wz::Color::blue);

				// Enable blending for transparent background on textures
				wz::RenderCommand::ToggleBlending(true);

				return;
			}
			case wz::EventType::app_update:
			{
				// Cast into respective event type to get the event data
				auto& updateEvent = (const wz::AppUpdateEvent&)e;

				// Move the mario texture each frame based on delta time
				component->position += vec2(5.f * updateEvent.GetDeltaTime());

				return;
			}
			case wz::EventType::app_render:
			{
				// Clear the render pipeline and draw anything rendered to the window
				wz::RenderCommand::Clear();

				// Begin the renderer to be ready to draw a scene using given shader
				wz::Renderer2D::Begin(component->hShader, component->projection);

				// Submit the mario texture for drawing at the position, a scale of 1, a 
				// rotation of 1 and with the color white (to render with default colors)
				wz::Renderer2D::SubmitImage(component->hMarioTexture, component->position, vec2(1.f), 0, wz::Color::white);

				// Tell the renderer we are done submitting and render all submissions
				wz::Renderer2D::End();

				return;
			}
			case wz::EventType::window_resize:
			{
				// Update the projection to the new window size
				auto& window = wz::Application::Get().GetWindow();
				component->projection = glm::ortho<float>(0, window.GetWidth(), 0, window.GetHeight());
			}
		}
	}

};

class MyApp : public wz::Application
{
public:

	virtual void Init() override
	{
		// Hide core trace messages
		wz::Log::SetCoreLogLevel(LOG_LEVEL_DEBUG);

		WZ_DEBUG("Initializing!");

		// Create an entity with and ExampleComponent
		m_clientEcs.CreateEntity<ExampleComponent>();

		// Register the ExampleSystem to be notified on events
		m_clientSystems.AddSystem<ExampleSystem>();
	}

	virtual void Shutdown() override
	{
		WZ_DEBUG("Shutting down!");
	}
};

// This is essential for the Core library to get an instance of the app!
wz::Application* CreateApplication()
{
	return new MyApp();
}
`

Now, to generate projects and link everything you will need a premake script. Create a file called premake5.lua in the root directory, above Wizzy and MyProject. The script should look something like this (windows):
`
include "Wizzy/premake5.lua"

project "MyInteractiveProject"
	location "MyInteractiveProject"
	kind "ConsoleApp"
	language "C++"
	architecture "x64"
	
	targetdir ("bin/" .. output_dir .. "/%{prj.name}")
	objdir ("bin-int/" .. output_dir .. "/%{prj.name}")
	
	pchheader "pch.h"
	pchsource "MyInteractiveProject/pch.cpp"
	
	files
	{
		"MyInteractiveProject/**.h",
		"MyInteractiveProject/**.cpp"
	}
	
	removefiles
	{
		"%{prj.name}/src/Wizzy/Platform/**"
	}
	
	includedirs
	{
		"Wizzy/Wizzy/src",
		"Wizzy/%{include_dir.imgui}",
		"Wizzy/%{include_dir.glm}",
		"Wizzy/%{include_dir.spdlog}",
		"Wizzy/%{include_dir.glfw}",
		"Wizzy/%{include_dir.glad}",
		"Wizzy/%{include_dir.glm}",
		"Wizzy/%{include_dir.stb}",
		"Wizzy/%{include_dir.lua}",
		"Wizzy/%{include_dir.ulib}",
		"Wizzy/%{include_dir.luabridge}",
		"Wizzy/%{include_dir.pfd}",
		"Wizzy/%{include_dir.ft}"
	}
	
	defines
	{
		"WZ_USE_OPENGL",
		"FREETYPE_STATIC"
	}
	
	links
	{
		"Wizzy",
		"imgui",
		"glfw",
		"glad",
		"lua"
	}
	
	
	-------- Windows
	cppdialect "C++17"
	staticruntime "On"
	systemversion "latest"
	
	links
	{
        	"opengl32.lib",
        	"glu32.lib",
		"%{wks.location}/Wizzy/vendor/freetype/windows/lib/freetype.lib"
	}

    	files
    	{
      		"%{prj.name}/src/Wizzy/platform/windows/**.h",
      		"%{prj.name}/src/Wizzy/platform/windows/**.cpp"
    	}

    	defines
    	{
      		"WZ_PLATFORM_WINDOWS",
      		"GLFW_INCLUDE_NONE"
    	}
	
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


`

To invoke the script with premake you can use the premake binary in the repo:
`Wizzy\vendor\bin\premake\windows\premake5.lua XXXX`
Replace 'XXXX' with whatever target you want to generate project files. Supported targets are:
-codelite          Generate CodeLite project files
-gmake             Generate GNU makefiles for POSIX, MinGW, and Cygwin
-gmake2            Generate GNU makefiles for POSIX, MinGW, and Cygwin
-vs2005            Generate Visual Studio 2005 project files
-vs2008            Generate Visual Studio 2008 project files
-vs2010            Generate Visual Studio 2010 project files
-vs2012            Generate Visual Studio 2012 project files
-vs2013            Generate Visual Studio 2013 project files
-vs2015            Generate Visual Studio 2015 project files
-vs2017            Generate Visual Studio 2017 project files
-vs2019            Generate Visual Studio 2019 project files

As you build the project (from the Wizzy workspace) and run, you should get this result:
<blockquote class="imgur-embed-pub" lang="en" data-id="a/ZvejSRo"  ><a href="//imgur.com/a/ZvejSRo">wizzy-mario</a></blockquote><script async src="//s.imgur.com/min/embed.js" charset="utf-8"></script>