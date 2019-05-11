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

namespace Wizzy {

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

		Renderer _renderer;

		Shader& _shader = *ResourceManagement::Get<Shader>("basicShader");
		Texture2D& _tex2d = *ResourceManagement::Get<Texture2D>("sprite");

		vec2 _pos1(2, 0);
		vec2 _pos2(250, 100);

		while (m_running) {
			m_window->OnFrameBegin();

			m_layerStack.UpdateLayers();

			_renderer.Draw(&_tex2d,
							&_shader,
							_ortho *
							glm::translate(glm::identity<mat4>(), vec3(_pos2, 0)));

			_renderer.Draw(&_tex2d, 
							&_shader, 
							_ortho *
							glm::translate(glm::identity<mat4>(), vec3(_pos1, 0)));

			float _speed = 1.f;

			if (Input::GetKey(WZ_KEY_W)) {
				_pos1.y += _speed;
			}
			if (Input::GetKey(WZ_KEY_A)) {
				_pos1.x -= _speed;
			}
			if (Input::GetKey(WZ_KEY_S)) {
				_pos1.y -= _speed;
			}
			if (Input::GetKey(WZ_KEY_D)) {
				_pos1.x += _speed;
			}

			_pos2 = Input::GetMousePos();
			_pos2.y = m_window->GetHeight() - _pos2.y;

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
