#include "wzpch.h"
#include "Application.h"
#include "Log.h"
#include "events\appevent.h"

namespace Wizzy {

	Application::Application() {
	}

	Application::~Application() {
	}

	void Application::Run() {
		
		m_window = std::unique_ptr<IWindow>(IWindow::Create());

		m_running = true;

		while (m_running) {
			m_window->OnFrameBegin();



			m_window->OnFrameEnd();
		}
	}

}
