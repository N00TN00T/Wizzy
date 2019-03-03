#pragma once

#include "Core.h"
#include "IWindow.h"
#include "events/Event.h"
#include "events/AppEvent.h"

namespace Wizzy {

	class WZ_API Application {
	public:
		Application();
		~Application();

		void OnEvent(Event& e);

		void Run();

	private:
		bool OnWindowClose(WindowCloseEvent& e);
	protected:
		std::unique_ptr<IWindow> m_window;

	private:
		bool m_running;
	};

	Application* CreateApplication();
}
