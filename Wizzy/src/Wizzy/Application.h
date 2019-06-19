#pragma once

#include "Wizzy/Events/AppEvent.h"
#include "Wizzy/IWindow.h"

namespace Wizzy {

	struct ApplicationProps {
		string appName;
		string appVersion;

		ApplicationProps(const string& appName = "Wizzy App",
						 const string& appVersion = "") :
						 appName(appName),
						 appVersion(appVersion) {}
	};

	class WZ_API Application {
	public:
		Application();
		virtual ~Application();

		void OnEvent(Event& e);

		void Run();

		inline IWindow& GetWindow() { return *m_window; }

		inline ApplicationProps& GetProps() { return m_props; }
		inline void SetProps(const ApplicationProps& props) { m_props = props; }

        inline bool WantRestart() const { return m_wantRestart; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	protected:
		std::unique_ptr<IWindow> m_window;

	private:
		bool m_running;
        bool m_wantRestart = false;
		ApplicationProps m_props;
		ecs::ECSManager m_engineManager;
		ecs::SystemLayer m_mainLayer;

	public:
		inline static Application& Get() { return *s_instance; }
	private:
		static Application *s_instance;
	};
}
