#pragma once

#include "Wizzy/Core.h"
#include "Wizzy/Events/AppEvent.h"
#include "Wizzy/IWindow.h"
#include "Wizzy/LayerStack.h"
#include "Wizzy/imgui/ImguiLayer.h"

namespace Wizzy {

	struct ApplicationProps {
		string appName;
		string appVersion;

		ApplicationProps(const string& appName = "Wizzy App",
						 const string& appVersion = "") :
						 appName(appName),
						 appVersion(appVersion) {}
	};

	class  Application {
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

		inline ApplicationProps& GetProps() { return m_props; }
		inline void SetProps(const ApplicationProps& props) { m_props = props; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	protected:
		std::unique_ptr<IWindow> m_window;
		ImguiLayer *m_imguiLayer;

	private:
		bool m_running;
		LayerStack m_layerStack;
		ApplicationProps m_props;

	public:
		inline static Application& Get() { return *s_instance; }
	private:
		static Application *s_instance;
	};

	Application* CreateApplication();
}
