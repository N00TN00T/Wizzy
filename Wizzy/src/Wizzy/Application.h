#pragma once

#include "Core.h"
#include "IWindow.h"

namespace Wizzy {

	class WZ_API Application {
	public:
		Application();
		~Application();

		void Run();

	protected:
		std::unique_ptr<IWindow> m_window;

	private:
		bool m_running;
	};

	Application* CreateApplication();
}
