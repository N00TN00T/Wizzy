#pragma once

#include "Core.h"

namespace Wizzy {
	class WZ_API Application {
	public:
		Application();
		~Application();

		void Run();
	};

	Application* CreateApplication();
}
