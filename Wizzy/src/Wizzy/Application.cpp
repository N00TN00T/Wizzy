#include "wzpch.h"

#include <imgui.h>

#include "Wizzy/Application.h"
#include "Wizzy/Log.h"
#include "Wizzy/Events/AppEvent.h"
#include "Wizzy/ImGuiSystem.h"
#include "Wizzy/Stopwatch.h"
#include "Wizzy/JobSystem/JobSystem.h"
#include "Wizzy/Renderer/Renderer2D.h"
#include "Wizzy/Resource/ResourceManagement.h"

#include "Wizzy/Instrumentor.h"


#define DISPATCH_EVENT_LOCAL(eType, ...) { this->OnEvent(eType(__VA_ARGS__)); }

namespace Wizzy {

	Application* Application::s_instance(nullptr);


	Application::Application() {

	}
	Application::~Application() {

	}

	void Application::OnEvent(Event& e) {
		//WZ_PROFILE_FUNCTION();
		EventDispatcher _dispatcher(e);

		_dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) {
			m_running = false;
			return false;
		});

		WZ_CORE_TRACE("Notifying client systems '{0}'", e);
		m_clientEcs.NotifySystems(m_clientSystems, e);
		WZ_CORE_TRACE("Notifying engine systems '{0}'", e);
		m_engineEcs.NotifySystems(m_engineSystems, e);

		for (auto& fn : m_eventCallbacks)
		{
			fn(e);
		}
	}

	void Application::Run() 
	{
		WZ_PROFILE_BEGIN_SESSION("Wizzy", "benchmark.json");
		WZ_PROFILE_FUNCTION();
		s_instance = this;

		string _wndTitle = m_props.appName
			+ (m_props.appVersion.size() > 0 ?
				" v" + m_props.appVersion
				: "");

#ifdef WZ_CONFIG_DEBUG
		_wndTitle += " (Wizzy version '" + WZ_VERSION + "') - Debug";
#elif defined(WZ_CONFIG_RELEASE)
		_wndTitle += " (Wizzy version '" + WZ_VERSION + "') - Release";
#elif !defined(WZ_CONFIG_DIST)
		_wndTitle += " (Wizzy version '"
			+ WZ_VERSION
			+ "') - Configuration not defined";
#endif


		WindowProps _windowProps = WindowProps(_wndTitle,
			1600,
			900);

		m_window = std::shared_ptr<IWindow>(IWindow::Create(_windowProps));
		m_window->SetEventCallback(WZ_BIND_FN(Application::OnEvent));

		m_running = true;

		ImGuiComponent _imguiComponent;
		Wizzy::IComponent* _comps[] = {
			&_imguiComponent
		};
		Wizzy::StaticCId _ids[] = {
			ImGuiComponent::staticId
		};

		m_engineEcs.CreateEntity(_comps, _ids, 1);

		m_engineSystems.AddSystem<ImGuiSystem>();

		JobSystem::Init();

		ResourceManagement::SetResourceDir("");

		this->Init();

		DISPATCH_EVENT_LOCAL(AppInitEvent);

		Renderer2D::Init();

		std::future<void> as;
		int64 f = 0;
		
		while (m_running) {
			WZ_PROFILE_SCOPE("App loop");

			DISPATCH_EVENT_LOCAL(AppFrameBeginEvent, m_window->GetDeltaTime());

			DISPATCH_EVENT_LOCAL(AppUpdateEvent, m_window->GetDeltaTime());

			DISPATCH_EVENT_LOCAL(AppRenderEvent);

			DISPATCH_EVENT_LOCAL(AppFrameEndEvent, m_window->GetDeltaTime());
			

			m_window->OnFrameEnd();

		}
		DISPATCH_EVENT_LOCAL(AppShutdownEvent, 0);

		this->Shutdown();

		Renderer2D::Shutdown();

		WZ_CORE_INFO("WIZZY APPLICATION END");

		WZ_PROFILE_END_SESSION();
	}
	void Application::AddEventCallback(const std::function<void(Event & e)>& fn)
	{
		m_eventCallbacks.push_back(fn);
	}
}
