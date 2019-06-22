#pragma once

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

	protected:
		virtual void Init() = 0;

	protected:
		std::unique_ptr<IWindow> m_window;
		ecs::ECSManager m_ecs;
		ecs::SystemLayer m_engineSystems;
		ecs::SystemLayer m_gameSystems;

	private:
		bool m_running;
        bool m_wantRestart = false;
		ApplicationProps m_props;

	public:
		inline static Application& Get() { return *s_instance; }
	private:
		static Application *s_instance;
	};
}
