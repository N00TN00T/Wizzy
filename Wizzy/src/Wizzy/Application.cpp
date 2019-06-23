#include "wzpch.h"
#include "Application.h"
#include "Log.h"
#include "Events/AppEvent.h"
#include "Wizzy/Input.h"

#include <imgui.h>

#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include "Wizzy/Resources/Shader.h"
#include "Wizzy/Resources/ResourceManagement.h"
#include "Wizzy/LuaContext.h"
#include "Wizzy/ImGuiSystem.h"
#include "Wizzy/Renderer/API.h"
#include "Wizzy/Utils.h"

#define DISPATCH_EVENT_LOCAL(eType, ...) { auto _e = eType(__VA_ARGS__); this->OnEvent(_e); } [](){return 0;}()

namespace Wizzy {

	Application* Application::s_instance(nullptr);

	struct RenderEnvironmentComponent
		: public ecs::Component<RenderEnvironmentComponent> {
		mat4 view;
		mat4 projection;
	};

	class RendererSetupSystem
		: public ecs::System {
	public:
		RendererSetupSystem() {
			AddComponentType<RenderEnvironmentComponent>();
	        AddComponentType<ImGuiComponent>();

			Subscribe((int32)AppFrameBeginEvent::GetStaticType());
	        Subscribe((int32)AppUpdateEvent::GetStaticType());
			Subscribe((int32)AppFrameEndEvent::GetStaticType());
	        Subscribe((int32)WindowResizeEvent::GetStaticType());
		}

		void BeginRenderer(const RenderEnvironmentComponent& env) const {
			WZ_CORE_TRACE("Beginning renderer...");
			RenderCommand::Clear();
			Renderer::Begin(env.view, env.projection);
		}

		void EndRenderer(const RenderEnvironmentComponent& env) const {
			WZ_CORE_TRACE("Ending Renderer...");
			Renderer::End();
		}

	    void OnWindowResize(const WindowResizeEvent& e, RenderEnvironmentComponent& env) const {
	        env.projection =
	            glm::perspective<float>(to_radians(90), e.GetX() / e.GetY(), .01f, 10000);
	        RenderCommand::SetViewport(0, 0, e.GetX(), e.GetY());
	    }

	    void OnUpdate(RenderEnvironmentComponent& env,
	                    ImGuiComponent& imguiComponent) const {
	        imguiComponent.imguiLayers.Push([&](){
	            ImGui::Begin("Camera");

	            ImGui::DragFloat3("Position", glm::value_ptr(env.view) + 12, 0.05f);
	            ImGui::DragFloat("FOV", glm::value_ptr(env.view) + 16, 0.01f);

	            ImGui::End();
	        });
	    }

		virtual
	    void OnEvent(const void* eventHandle,
							 ecs::ComponentGroup& components) const override {
			const Event& _e = *static_cast<const Event*>(eventHandle);
			RenderEnvironmentComponent& _env = *components.Get<RenderEnvironmentComponent>();
	        ImGuiComponent& _imguiComponent = *components.Get<ImGuiComponent>();

			switch (_e.GetEventType()) {
				case EventType::app_frame_begin:
					BeginRenderer(_env);
					break;
	            case EventType::app_update:
					OnUpdate(_env, _imguiComponent);
					break;
				case EventType::app_frame_end:
					EndRenderer(_env);
					break;
	            case EventType::window_resize:
	            {
	                const WindowResizeEvent& _windowResizeEvent =
	                        *static_cast<const WindowResizeEvent*>(eventHandle);
					OnWindowResize(_windowResizeEvent, _env);
					break;
	            }
				default: break;
			}
		}
	};


	Application::Application() {

	}
	Application::~Application() {

	}

	void Application::OnEvent(Event& e) {
		EventDispatcher _dispatcher(e);

		_dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) {
			m_running = false;
			return false;
		});

		void *_eHandle = static_cast<void*>(&e);
		WZ_CORE_TRACE("Notifying engine systems '{0}'", e);
		m_ecs.NotifySystems(m_engineSystems, _eHandle, (int32)e.GetEventType());
		WZ_CORE_TRACE("Notifying game systems '{0}'", e);
		m_ecs.NotifySystems(m_gameSystems, _eHandle, (int32)e.GetEventType());
	}

	void Application::Run() {

		s_instance = this;

		string _wndTitle = m_props.appName
			+ (m_props.appVersion.size() > 0 ?
				" v" + m_props.appVersion
				: "");

#ifdef WZ_CONFIG_DEBUG
		_wndTitle += " (Wizzy version '" + WZ_VERSION + "') - Debug";
#elif defined(WZ_CONFIG_RELEASE)
		_wndTitle += " (Wizzy version '" + WZ_VERSION + "') - Release";
#elif !defined(WZ_CONFIG_DIST)
		_wndTitle += " (Wizzy version '"
			+ WZ_VERSION
			+ "') - Configuration not defined";
#endif


		WindowProps _windowProps = WindowProps(_wndTitle,
			1600,
			900);

		m_window = std::unique_ptr<IWindow>(IWindow::Create(_windowProps));
		m_window->SetEventCallback(WZ_BIND_FN(Application::OnEvent));

		m_running = true;

		RenderEnvironmentComponent _envComp;
		_envComp.view = glm::lookAt(vec3(0, 0, -10), vec3(0, 0, 0), vec3(0, 1, 0));
		_envComp.projection =
                        glm::perspectiveFov<float>(90,
                                                    m_window->GetWidth(),
                                                    m_window->GetHeight(),
                                                    .1f,
                                                    10000);
		ImGuiComponent _imguiComponent;
		ecs::IComponent* _comps[] = {
			&_envComp, &_imguiComponent
		};
		ecs::StaticCId _ids[] = {
			_envComp.staticId, ImGuiComponent::staticId
		};

		m_ecs.CreateEntity(_comps, _ids, 2);

		m_engineSystems.AddSystem<RendererSetupSystem>();
		m_engineSystems.AddSystem<ImGuiSystem>();

		this->Init();

		while (m_running) {
			m_window->OnFrameBegin();
			DISPATCH_EVENT_LOCAL(AppFrameBeginEvent, m_window->GetDeltaTime());

			DISPATCH_EVENT_LOCAL(AppUpdateEvent, m_window->GetDeltaTime());

			DISPATCH_EVENT_LOCAL(AppRenderEvent);

			DISPATCH_EVENT_LOCAL(AppFrameEndEvent, m_window->GetDeltaTime());
			m_window->OnFrameEnd();
		}
	}
}
