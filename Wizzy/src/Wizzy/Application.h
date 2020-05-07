#pragma once

#include "Wizzy/Ecs/ECSManager.h"
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

		void DispatchEvent(Event& e);

		void Run();

		inline IWindow& GetWindow() { return *m_window; }

		inline ApplicationProps& GetProps() { return m_props; }
		inline void SetProps(const ApplicationProps& props) { m_props = props; }

        inline bool WantRestart() const { return m_wantRestart; }

	protected:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		void AddEventCallback(const std::function<void(Event & e)>& fn);

	private:
		void OnEventBase(const Event& e);

	protected:
		std::shared_ptr<IWindow> m_window;
		Wizzy::ECSManager m_ecs;
		Wizzy::SystemLayer m_systems;

	private:
		bool m_running;
        bool m_wantRestart = false;
		ApplicationProps m_props;
		std::vector<std::function<void(Event & e)>> m_eventCallbacks;

	public:
		inline static Application& Get() { return *s_instance; }
	private:
		static Application *s_instance;
	};
}
