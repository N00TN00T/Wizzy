#pragma once

#include "Wizzy/Ecs/Ecs.h"
#include "Wizzy/Ecs/SystemLayout.h"
#include "Wizzy/Core/IWindow.h"

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
		virtual void Init()          = 0;
		virtual void LoadResources() = 0;
		virtual void Shutdown()      = 0;

		void AddEventCallback(const std::function<void(Event & e)>& fn);

	protected:
		std::shared_ptr<IWindow> m_window;
		EcsInstance m_ecs;
		SystemLayer m_systemLayer;

	private:
		bool m_running;
        bool m_wantRestart = false;
		ApplicationProps m_props;
		std::vector<std::function<void(Event & e)>> m_eventCallbacks;
		SystemLayer m_coreLayer;

	public:
		inline static Application& Get() { return *s_instance; }
	private:
		static Application *s_instance;
	};
}
