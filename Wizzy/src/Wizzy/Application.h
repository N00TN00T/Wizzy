#pragma once

#include "Wizzy/Core.h"
#include "Wizzy/events/AppEvent.h"
#include "Wizzy/IWindow.h"
#include "LayerStack.h"

namespace Wizzy {

	class WZ_API Application {
	public:
		Application();
		~Application();

		void OnEvent(Event& e);

		void Run();

		void PushLayer(Layer *layer);
		void PushOverlay(Layer *overlay);
		void PopLayer(Layer *layer);
		void PopOverlay(Layer *overlay);

	private:
		bool OnWindowClose(WindowCloseEvent& e);
	protected:
		std::unique_ptr<IWindow> m_window;

	private:
		bool m_running;
		LayerStack m_layerStack;
	};

	Application* CreateApplication();
}
