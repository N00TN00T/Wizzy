#include "wzpch.h"
#include "Application.h"
#include "Log.h"
#include "Events/AppEvent.h"
#include "Wizzy/Input.h"

#include <imgui.h>

#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include "Wizzy/Resources/Shader.h"
#include "Wizzy/Resources/ResourceManagement.h"
#include "Wizzy/LuaContext.h"
#include "Wizzy/ImGuiSystem.h"
#include "Wizzy/Renderer/API.h"

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

		void *_eHandle = static_cast<void*>(&e);
		m_ecs.NotifySystems(m_engineSystems, _eHandle, (int32)e.GetEventType());
		m_ecs.NotifySystems(m_gameSystems, _eHandle, (int32)e.GetEventType());
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

		ecs::IComponent *_imguiComponents[] = {
			&_imguiComponent
		};
		ecs::StaticCId _imguiIds[] = {
			ImGuiComponent::staticId
		};

		m_ecs.CreateEntity(_imguiComponents, _imguiIds, 1);

		m_engineSystems.AddSystem<ImGuiSystem>();

		this->Init();

		while (m_running) {
			m_window->OnFrameBegin();
			DISPATCH_EVENT_LOCAL(AppFrameBeginEvent, m_window->GetDeltaTime());

			DISPATCH_EVENT_LOCAL(AppUpdateEvent, m_window->GetDeltaTime());

			DISPATCH_EVENT_LOCAL(AppRenderEvent);

			DISPATCH_EVENT_LOCAL(AppFrameEndEvent, m_window->GetDeltaTime());
			m_window->OnFrameEnd();
		}
	}
}
