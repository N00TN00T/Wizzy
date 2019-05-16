#include "wzpch.h"
#include "Application.h"
#include "Log.h"
#include "Events/AppEvent.h"
#include "Wizzy/Input.h"

#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include <glad/glad.h>
#include <imgui.h>

#include "Wizzy/Resources/Shader.h"
#include "Wizzy/Resources/Texture2D.h"
#include "Wizzy/Resources/ResourceManagement.h"
#include "Wizzy/Renderer.h"
#include "Wizzy/ECS/ECS.h"

namespace Wizzy {

	struct Position
		: public Component<Position> {
		vec2 value;
	};
	struct Sprite
		: public Component<Sprite> {
		Texture2D *texture;
		Shader *shader;
	};
	struct Projection
		: public Component<Projection> {
		mat4 ortho;
	};
	struct Text
		: public Component<Text> {
		const char *value;
	};
	struct ThingData
		: public Component<ThingData> {
		vec2 speed;
		vec2 max;
		vec2 min;
		vec2 dir;
	};
	struct PlayerData
		: public Component<PlayerData> {
		float moveSpeed;
	};

	class ThingSystem
		: public System {
	public:
		ThingSystem() {
			AddComponentType<Position>();
			AddComponentType<Sprite>();
			AddComponentType<ThingData>();
		}

		virtual void OnUpdate(const float& deltaTime, IComponent** components) const {
			Position& _position = *static_cast<Position*>(components[0]);
			Sprite& _sprite = *static_cast<Sprite*>(components[1]);
			ThingData& _thing = *static_cast<ThingData*>(components[2]);

			_position.value += (_thing.speed * _thing.dir) * deltaTime;

			if (_position.value.x >= _thing.max.x) _thing.dir.x = -1;
			if (_position.value.x <= _thing.min.x) _thing.dir.x = 1;
			if (_position.value.y >= _thing.max.y) _thing.dir.y = -1;
			if (_position.value.y <= _thing.min.y) _thing.dir.y = 1;
		}
	};

	class PlayerSystem
		: public System{
	public:
		PlayerSystem() {
			AddComponentType<Position>();
			AddComponentType<Sprite>();
			AddComponentType<PlayerData>();
		}

		virtual void OnUpdate(const float& deltaTime, IComponent** components) const {
			Position& _position = *static_cast<Position*>(components[0]);
			Sprite& _sprite = *static_cast<Sprite*>(components[1]);
			PlayerData& _player = *static_cast<PlayerData*>(components[2]);

			float _move = _player.moveSpeed * deltaTime;

			if (Input::GetKey(WZ_KEY_W)) {
				_position.value.y += _move;
			}
			if (Input::GetKey(WZ_KEY_A)) {
				_position.value.x -= _move;
			}
			if (Input::GetKey(WZ_KEY_S)) {
				_position.value.y -= _move;
			}
			if (Input::GetKey(WZ_KEY_D)) {
				_position.value.x += _move;
			}
		}
	};

	class RenderSystem
		: public System {

	public:
		RenderSystem() {
			AddComponentType<Position>();
			AddComponentType<Sprite>();
			AddComponentType<Projection>();
		}

		virtual void OnUpdate(const float & deltaTime, IComponent ** components) const override {

			Position& _position = *static_cast<Position*>(components[0]);
			Sprite& _sprite = *static_cast<Sprite*>(components[1]);
			Projection& _projection = *static_cast<Projection*>(components[2]);

			Renderer _renderer;

			mat4 _translation = glm::translate(glm::identity<mat4>(), vec3(_position.value, 0));
			_renderer.Draw(_sprite.texture, _sprite.shader, _projection.ortho * _translation);
		}
	};


	Application *Application::s_instance;

	Application::Application() {
		WZ_CORE_ASSERT(!s_instance, "The Application class should be used as a singleton, but a second instance was created");
	}

	Application::~Application() {
	}

	void Application::OnEvent(Event& e) {
		EventDispatcher _dispatcher(e);
		_dispatcher.Dispatch<WindowCloseEvent>(WZ_BIND_FN(Application::OnWindowClose));
		_dispatcher.Dispatch<WindowResizeEvent>(WZ_BIND_FN(Application::OnWindowResize));

		m_layerStack.OnEvent(e);
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

		m_imguiLayer = new ImguiLayer();
		PushOverlay(m_imguiLayer);

		m_running = true;
		ResourceManagement::SetResourcePath(string(BASE_DIR) + "Resources/");

		ResourceManagement::Import<Texture2D> (
											"sprite.png",
											"sprite"
										);
		ResourceManagement::Import<Shader> (
											"basic.shader",
											"basicShader"
										);

		mat4 _ortho = glm::ortho<float>(0, 1600, 0, 900);

		Shader& _shader = *ResourceManagement::Get<Shader>("basicShader");
		Texture2D& _tex2d = *ResourceManagement::Get<Texture2D>("sprite");

		ECS _ecs;;

		Position	_position;
		Sprite		_sprite;
		Projection	_projection;
		ThingData	_thingData;
		PlayerData _playerData;

		srand(time(NULL));

		for (u32 i = 0; i < 1000; i++) {
			_position.value.x = rand() % 1600;
			_position.value.y = rand() % 900;

			_sprite.texture = &_tex2d;
			_sprite.shader = &_shader;

			_projection.ortho = _ortho;

			_thingData.speed = vec2(150 - rand() % 15, 150 - rand() % 15);
			_thingData.max = vec2(1600, 900);
			_thingData.min = vec2(0, 0);
			_thingData.dir = vec2(rand() % 2 == 1 ? 1 : -1, rand() % 2 == 1 ? 1 : -1);

			IComponent* _components[] = {
				&_position, &_sprite, &_projection,
				&_thingData
			};
			StaticCId _ids[] = {
				_position.staticId, _sprite.staticId, _projection.staticId,
				_thingData.staticId
			};

			_ecs.CreateEntity(_components, _ids, 4);
		}

		SystemLayer _systems1;

		_systems1.AddSystem<PlayerSystem>();
		_systems1.AddSystem<ThingSystem>();

		SystemLayer _systems2;
		_systems2.AddSystem<RenderSystem>();

		while (m_running) {
			m_window->OnFrameBegin();

			WZ_CORE_DEBUG("SYSTEMS 1");
			_ecs.UpdateSystems(_systems1, m_window->GetDeltaTime());
			WZ_CORE_DEBUG("SYSTEMS 2");
			_ecs.UpdateSystems(_systems2, m_window->GetDeltaTime());

			m_layerStack.UpdateLayers();

			m_imguiLayer->Begin();
			m_layerStack.OnImguiRender();
			m_imguiLayer->End();

			m_window->OnFrameEnd();
		}
	}

	void Application::PushLayer(Layer * layer) {
		m_layerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer * overlay) {
		m_layerStack.PushOverlay(overlay);
	}

	void Application::PopLayer(Layer * layer) {
		m_layerStack.PopLayer(layer);
	}

	void Application::PopOverlay(Layer * overlay) {
		m_layerStack.PopOverlay(overlay);
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
