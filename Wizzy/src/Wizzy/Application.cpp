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

			WZ_TRACE("PLAYER");
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
		ResourceManagement::SetResourcePath("C:\\dev\\Wizzy\\Resources\\");

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

		ECS _ecs;

		Position	_playerPos;	
		_playerPos.value.x = 51; 
		_playerPos.value.y = 55;
		Sprite		_playerSprite; 
		_playerSprite.texture = &_tex2d; 
		_playerSprite.shader = &_shader;
		Projection	_playerProjection; 
		_playerProjection.ortho = _ortho;
		PlayerData	_playerData;
		_playerData.moveSpeed = 100;

		IComponent* _playerComponents[] = {
			&_playerPos, &_playerSprite, &_playerProjection, 
			&_playerData
		};
		StaticCId _playerIds[] = {
			_playerPos.staticId, _playerSprite.staticId, _playerProjection.staticId,
			_playerData.staticId
		};

		//_ecs.CreateEntity(_playerComponents, _playerIds, 4);

		Position	_thingPos;
		_thingPos.value.x = 850;
		_thingPos.value.y = 200;
		Sprite		_thingSprite;
		_thingSprite.texture = &_tex2d;
		_thingSprite.shader = &_shader;
		Projection	_thingProjection;
		_thingProjection.ortho = _ortho;
		ThingData	_thingData;
		_thingData.speed = vec2(10, 10);
		_thingData.dir = vec2(1, 1);
		_thingData.max = vec2(1600, 900);
		_thingData.min = vec2(0, 0);

		IComponent* _thingComponents[] = {
			&_thingPos, &_thingSprite, &_thingProjection,
			&_thingData
		};
		StaticCId _thingIds[] = {
			_thingPos.staticId, _thingSprite.staticId, _thingProjection.staticId,
			_thingData.staticId
		};

		_ecs.CreateEntity(_thingComponents, _thingIds, 4);
		
		SystemLayer _systems;

		_systems.AddSystem<PlayerSystem>();
		_systems.AddSystem<ThingSystem>();
		_systems.AddSystem<RenderSystem>();

		while (m_running) {
			m_window->OnFrameBegin();

			m_layerStack.UpdateLayers();

			_ecs.UpdateSystems(_systems, 1.0f / 60.0f);

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
		//glViewport(0, 0, e.GetX(), e.GetY());
		
		return false;
	}
}
