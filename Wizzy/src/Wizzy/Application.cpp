#include "wzpch.h"
#include "Application.h"
#include "Log.h"
#include "events\appevent.h"

namespace Wizzy {

	Application::Application() {
	}

	Application::~Application() {
	}

	void Application::OnEvent(Event& e){
		EventDispatcher _dispatcher(e);
		_dispatcher.Dispatch<WindowCloseEvent>(WZ_BIND_FN(Application::OnWindowClose));
	}

	void Application::Run() {
		
		m_window = std::unique_ptr<IWindow>(IWindow::Create());
		m_window->SetEventCallback(WZ_BIND_FN(Application::OnEvent));

		m_running = true;

		while (m_running) {
			m_window->OnFrameBegin();



			m_window->OnFrameEnd();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_running = false;
		return false;
	}
}
