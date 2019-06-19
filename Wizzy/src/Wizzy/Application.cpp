#include "wzpch.h"
#include "Application.h"
#include "Log.h"
#include "Events/AppEvent.h"
#include "Wizzy/Input.h"

#include <glad/glad.h>
#include <imgui.h>

#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include "Wizzy/Resources/Shader.h"
#include "Wizzy/Resources/ResourceManagement.h"
#include "Wizzy/LuaContext.h"
#include "Wizzy/ImGuiSystem.h"
#include "Wizzy/Renderer/API.h"

namespace Wizzy {

	struct GameComponent
		: public ecs::Component<GameComponent> {
		GameComponent(const GameComponent& src) { // TODO: wtf?
			gameManager = src.gameManager;
			systems = src.systems;
		}
		ecs::ECSManager gameManager;
		ecs::SystemLayer systems;
	};

	class GameSystem
		: public ecs::System {
	public:

		GameSystem() {
			AddComponentType<GameComponent>();

			SubscribeAll();
		}

		virtual void OnEvent(const void* eventHandle,
							 ecs::ComponentGroup& components) const override {
			const Event& _e = *static_cast<const Event*>(eventHandle);

			GameComponent& _game = *components.Get<GameComponent>();
			_game.gameManager.NotifySystems(_game.systems,
											eventHandle,
											(int32)_e.GetEventType());
		}
	};

	Application* Application::s_instance(nullptr);

	Application::Application() {

	}
	Application::~Application() {

	}

	void Application::OnEvent(Event& e) {
		EventDispatcher _dispatcher(e);

		_dispatcher.Dispatch<WindowCloseEvent>(WZ_BIND_FN(Application::OnWindowClose));
		_dispatcher.Dispatch<WindowResizeEvent>(WZ_BIND_FN(Application::OnWindowResize));

		void *_eHandle = static_cast<void*>(&e);
		m_engineManager.NotifySystems(m_mainLayer, _eHandle, (int32)e.GetEventType());
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
		ResourceManagement::SetResourcePath(string(BASE_DIR) + "Resources/");

		ResourceManagement::Import<Shader> (
											"basic.shader",
											"basicShader"
										);
        ResourceManagement::Import<Script> (
											"test.lua",
											"testScript"
										);

		mat4 _ortho = glm::ortho<float>(0, 1600, 0, 900);

		Shader& _shader = *ResourceManagement::Get<Shader>("basicShader");

		float _vertices[] = {
			-0.5f, -0.5f, 0.0f,		0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f,		0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f,		0.8f, 0.8f, 0.2f, 1.0f
		};

		VertexBuffer *_vbo = VertexBuffer::Create(_vertices, sizeof(_vertices));

		BufferLayout _layout = {
			{ ShaderDataType::FLOAT3, "vertexPosition" }
		};

		u32 _indices[] = { 0, 1, 2 };
		IndexBuffer *_ibo = IndexBuffer::Create(_indices, 3);

		ImGuiComponent _imguiComponent;

		ecs::IComponent *_imguiComponents[] = {
			&_imguiComponent
		};
		ecs::StaticCId _imguiIds[] = {
			ImGuiComponent::staticId
		};

		m_engineManager.CreateEntity(_imguiComponents, _imguiIds, 1);

		m_mainLayer.AddSystem<ImGuiSystem>();

		while (m_running) {
			m_window->OnFrameBegin();
			{
				auto _e = AppFrameBeginEvent(m_window->GetDeltaTime());
				this->OnEvent(_e);
			}

			{
				auto _e = AppUpdateEvent(m_window->GetDeltaTime());
				this->OnEvent(_e);
			}
			{
				auto _e = AppRenderEvent();
				this->OnEvent(_e);
			}

			{
				auto _e = AppFrameEndEvent(m_window->GetDeltaTime());
				this->OnEvent(_e);
			}
			m_window->OnFrameEnd();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_running = false;
		return false;
	}
	bool Application::OnWindowResize(WindowResizeEvent& e) {
		glViewport(0, 0, e.GetX(), e.GetY());

		return false;
	}
}
