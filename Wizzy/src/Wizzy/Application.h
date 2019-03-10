#pragma once

#include "Wizzy/Core.h"
#include "Wizzy/events/AppEvent.h"
#include "Wizzy/IWindow.h"
#include "Wizzy/LayerStack.h"
#include "Wizzy/imgui/ImguiLayer.h"

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

		inline IWindow& GetWindow() { return *m_window; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
	protected:
		std::unique_ptr<IWindow> m_window;
		ImguiLayer *m_imguiLayer;

	private:
		bool m_running;
		LayerStack m_layerStack;

	public:
		inline static Application& Get() { return *s_instance; }
	private:
		static Application *s_instance;
	};

	Application* CreateApplication();
}
