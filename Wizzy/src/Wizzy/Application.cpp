#include "wzpch.h"

#include <imgui.h>

#include "Wizzy/Application.h"
#include "Wizzy/Log.h"
#include "Wizzy/Events/AppEvent.h"
#include "Wizzy/ImGuiSystem.h"


#define DISPATCH_EVENT_LOCAL(eType, ...) { auto _e = eType(__VA_ARGS__); this->OnEvent(_e); } [](){return 0;}()

namespace Wizzy {

	Application* Application::s_instance(nullptr);


	Application::Application() {

	}
	Application::~Application() {

	}

	void Application::OnEvent(Event& e) {
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

	void Application::Run() {

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

		m_window = std::unique_ptr<IWindow>(IWindow::Create(_windowProps));
		m_window->SetEventCallback(WZ_BIND_FN(Application::OnEvent));

		m_running = true;

		ImGuiComponent _imguiComponent;
		ecs::IComponent* _comps[] = {
			&_imguiComponent
		};
		ecs::StaticCId _ids[] = {
			ImGuiComponent::staticId
		};

		m_engineEcs.CreateEntity(_comps, _ids, 1);

		m_engineSystems.AddSystem<ImGuiSystem>();

		this->Init();

		DISPATCH_EVENT_LOCAL(AppInitEvent);
		while (m_running) {
			m_window->OnFrameBegin();
			DISPATCH_EVENT_LOCAL(AppFrameBeginEvent, m_window->GetDeltaTime());

			DISPATCH_EVENT_LOCAL(AppUpdateEvent, m_window->GetDeltaTime());

			DISPATCH_EVENT_LOCAL(AppRenderEvent);

			DISPATCH_EVENT_LOCAL(AppFrameEndEvent, m_window->GetDeltaTime());

			m_window->OnFrameEnd();
		}

		this->Shutdown();
	}
}
