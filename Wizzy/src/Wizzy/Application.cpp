#include "wzpch.h"
#include "Application.h"
#include "Log.h"
#include "events/AppEvent.h"
#include "Wizzy/Input.h"

#include <GL/glew.h>

namespace Wizzy {

	Application *Application::s_instance;

	Application::Application() {
	}

	Application::~Application() {
	}

	void Application::OnEvent(Event& e){
		EventDispatcher _dispatcher(e);
		_dispatcher.Dispatch<WindowCloseEvent>(WZ_BIND_FN(Application::OnWindowClose));

		m_layerStack.OnEvent(e);
	}

	void Application::Run() {
		
		s_instance = this;

		string _wndTitle = m_props.appName + (m_props.appVersion.size() > 0 ? " v" + m_props.appVersion
												: "");

#ifdef WZ_CONFIG_DEBUG
		_wndTitle += " (Wizzy version '" + WZ_VERSION + "') - Debug";
#elif defined(WZ_CONFIG_RELEASE)
		_wndTitle += " (Wizzy version '" + WZ_VERSION + "') - Release";
#elif !defined(WZ_CONFIG_DIST)
		_wndTitle += " (Wizzy version '" + WZ_VERSION + "') - Configuration not defined";
#endif


		WindowProps _windowProps = WindowProps(	_wndTitle,
												1600,
												900 );

		m_window = std::unique_ptr<IWindow>(IWindow::Create(_windowProps));
		m_window->SetEventCallback(WZ_BIND_FN(Application::OnEvent));

		m_imguiLayer = new ImguiLayer();
		PushOverlay(m_imguiLayer);

		m_running = true;

		while (m_running) {
			m_window->OnFrameBegin();

			m_layerStack.UpdateLayers();

			m_imguiLayer->Begin();
			m_layerStack.OnImguiRender();
			m_imguiLayer->End();

			m_window->OnFrameEnd();
		}
	}

	void Application::PushLayer(Layer * layer) {
		m_layerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer * overlay) {
		m_layerStack.PushOverlay(overlay);
	}

	void Application::PopLayer(Layer * layer) {
		m_layerStack.PopLayer(layer);
	}

	void Application::PopOverlay(Layer * overlay) {
		m_layerStack.PopOverlay(overlay);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_running = false;
		return false;
	}
}
