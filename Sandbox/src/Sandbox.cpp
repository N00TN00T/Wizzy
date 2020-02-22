#include "spch.h"

#include "Sandbox.h"

EngineManagerSystem::EngineManagerSystem()
{
	AddComponentType<EngineData>();

	Subscribe(wz::EventType::app_init);
	Subscribe(wz::EventType::app_frame_begin);
	Subscribe(wz::EventType::app_update);
	Subscribe(wz::EventType::app_render);
	Subscribe(wz::EventType::app_frame_end);
	Subscribe(wz::EventType::app_shutdown);
}

void EngineManagerSystem::OnEvent(const wz::Event& e, ecs::ComponentGroup& components) const
{
	WZ_PROFILE_FUNCTION()
		EngineData& _testComp = *components.Get<EngineData>();

	wz::ResourceManagement::SetResourceDir(_testComp.resourcePath);

	switch (e.GetEventType())
	{
	case wz::EventType::app_init:
	{
		if (ulib::File::exists(_testComp.resourcePath + "ResourceList.rl"))
		{
			wz::ResourceManagement::LoadResourceList("ResourceList.rl");
		}

		_testComp.hndShader = (wz::Shader::Handle)wz::ResourceManagement::HandleOf("test.shader");
		_testComp.hndTexture = (wz::Texture::Handle)wz::ResourceManagement::HandleOf("test.png");

		/*auto hnd = wz::ResourceManagement::AddToResourceDir<wz::Texture>(ulib::File::directory_of(GetExecutablePath()) + "/../../../test/test.png", "textures/test.png", wz::Texture::GetTemplateProps());
		wz::ResourceManagement::Load(hnd);*/

		/*hnd = wz::ResourceManagement::AddToResourceDir<wz::Shader>(ulib::File::directory_of(GetExecutablePath()) + "/../../../test/test.shader", "textures/test.shader", wz::Shader::GetTemplateProps());
		wz::ResourceManagement::Load(hnd);*/

		wz::RenderCommand::SetClearColor(.5f, .5f, .5f, 1.f);

		wz::RenderCommand::SetCullMode(wz::WZ_CULL_NONE);

		wz::RenderCommand::ToggleDepthTesting(false);

		wz::RenderCommand::ToggleBlending(true);

		_testComp.window->SetVSync(false);

		_testComp.renderTarget = wz::RenderTargetPtr(wz::RenderTarget::Create(_testComp.window->GetWidth(), _testComp.window->GetHeight()));

		return;
	}
	case wz::EventType::app_frame_begin:
	{
		WZ_PROFILE_SCOPE("app_frame_begin");
		wz::RenderCommand::Clear();
		return;
	}
	case wz::EventType::app_update:
	{
		WZ_PROFILE_SCOPE("app_update");
		_testComp.timeSinceValidate += ((wz::AppUpdateEvent&)e).GetDeltaTime();
		if (_testComp.timeSinceValidate >= 1.f / _testComp.validationRate)
		{
			_testComp.ftStr = std::to_string(((wz::AppUpdateEvent&)e).GetDeltaTime() * 1000.f);
			_testComp.timeSinceValidate = 0.f;
			try
			{
				wz::ResourceManagement::Validate(false);
			}
			catch (const wz::ResourceFileMissingException & e)
			{
				wz::ResourceManagement::Delete(e.GetId());
				WZ_CORE_WARN("A file was missing and unregistered from the project:");
				wz::Log::SetExtra(false);
				WZ_CORE_WARN("    Exception: {0}", e.GetMessage());
				WZ_CORE_WARN("    File:      {0}", e.GetPath());
				wz::Log::SetExtra(true);
			}
			catch (const wz::Exception & e)
			{
				WZ_CORE_CRITICAL("Failed to validate resources beyond fixing!");
				wz::Log::SetExtra(false);
				WZ_CORE_CRITICAL(e.GetUnhandledMessage());
				WZ_BREAK;
				wz::Log::SetExtra(true);
			}

			wz::ResourceManagement::Validate(true);
		}

		if (wz::Input::GetKey(WZ_KEY_R))
		{
			_testComp.rotation += 20.f * ((wz::AppUpdateEvent&)e).GetDeltaTime();
		}

		if (wz::Input::GetKey(WZ_KEY_W))
		{
			_testComp.scale += 1.f * ((wz::AppUpdateEvent&)e).GetDeltaTime();
		}
		if (wz::Input::GetKey(WZ_KEY_S))
		{
			_testComp.scale -= 1.f * ((wz::AppUpdateEvent&)e).GetDeltaTime();
		}

		auto mouse = wz::Input::GetMousePos();
		_testComp.pos = mouse;
		ImGui::Begin("Performance");

		ImGui::LabelText("frametime", "%sms", _testComp.ftStr.c_str());

		ImGui::End();

		return;
	}
	case wz::EventType::app_render:
	{
		WZ_PROFILE_SCOPE("app_render");
		wz::Renderer2D::Begin(_testComp.hndShader, glm::ortho<float>(0, _testComp.window->GetWidth(), 0, _testComp.window->GetHeight(), -1, 1));

		wz::Renderer2D::SubmitImage(_testComp.hndTexture, _testComp.pos, _testComp.scale, _testComp.rotation, wz::Color::white);
		wz::Renderer2D::SubmitRect(wz::Rect(100, 100, 100, 100), wz::Color::blue);

		wz::Renderer2D::End(_testComp.renderTarget);

		ImGui::Begin("Game");

		ImGui::Image((ImTextureID)_testComp.renderTarget->GetTextureId(), ImGui::GetWindowSize(), { 0, 1 }, { 1, 0 });

		ImGui::End();

		return;
	}
	case wz::EventType::app_frame_end:
	{
		WZ_PROFILE_SCOPE("app_frame_end");
		return;
	}
	case wz::EventType::app_shutdown:
	{
		wz::ResourceManagement::WriteResourceList("ResourceList.rl");
		return;
	}
	}

}

Sandbox::Sandbox()
{
}

Sandbox::~Sandbox()
{
}

void Sandbox::Init()
{
	WZ_PROFILE_FUNCTION();
	wz::Log::SetCoreLogLevel(LOG_LEVEL_DEBUG);

	EngineData t;
	t.resourcePath = ulib::File::directory_of(GetExecutablePath()) + "/../../../res/";
	t.window = m_window;

	ecs::IComponent* tPtr = &t;

	m_clientEcs.CreateEntity(&tPtr, &t.staticId, 1);

	m_clientSystems.AddSystem<EngineManagerSystem>();
}


void Sandbox::Shutdown()
{
}
