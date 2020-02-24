#include "wzpch.h"

#include <imgui.h>

#include "Wizzy/Application.h"
#include "Wizzy/Log.h"
#include "Wizzy/Events/AppEvent.h"
#include "Wizzy/ImGuiSystem.h"
#include "Wizzy/Stopwatch.h"

#include "Wizzy/Instrumentor.h"


#define DISPATCH_EVENT_LOCAL(eType, ...) { this->OnEvent(eType(__VA_ARGS__)); }

namespace Wizzy {

	Application* Application::s_instance(nullptr);


	Application::Application() {

	}
	Application::~Application() {

	}

	void Application::OnEvent(Event& e) {
		WZ_PROFILE_FUNCTION();
		EventDispatcher _dispatcher(e);

		_dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) {
			m_running = false;
			return false;
		});

		WZ_CORE_TRACE("Notifying client systems '{0}'", e);
		m_clientEcs.NotifySystems(m_clientSystems, e);
		WZ_CORE_TRACE("Notifying engine systems '{0}'", e);
		m_engineEcs.NotifySystems(m_engineSystems, e);
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

		this->Init();

		DISPATCH_EVENT_LOCAL(AppInitEvent);
		std::future<void> as;
		int64 f = 0;
		//std::thread* renderThread;
		while (m_running) {
			string frameStr = "FRAME #" + std::to_string(++f);
			WZ_PROFILE_SCOPE(frameStr.c_str());
			m_window->OnFrameBegin();

			DISPATCH_EVENT_LOCAL(AppFrameBeginEvent, m_window->GetDeltaTime());

			DISPATCH_EVENT_LOCAL(AppUpdateEvent, m_window->GetDeltaTime());

			DISPATCH_EVENT_LOCAL(AppRenderEvent);
			//WZ_DEBUG("hi");
			//as = std::async(std::launch::async, &Application::OnEvent, this, AppRenderEvent());

			//std::thread(&Application::OnEvent, this, AppRenderEvent()).detach();

			DISPATCH_EVENT_LOCAL(AppFrameEndEvent, m_window->GetDeltaTime());
			

			m_window->OnFrameEnd();

		}
		DISPATCH_EVENT_LOCAL(AppShutdownEvent, 0);

		this->Shutdown();
		WZ_PROFILE_END_SESSION();
	}
}
