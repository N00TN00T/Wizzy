#include "wzpch.h"
#include "Application.h"
#include "Log.h"
#include "Events/AppEvent.h"
#include "Wizzy/Input.h"

#include <glad/glad.h>
#include <imgui.h>
#include "Wizzy/Resources/Shader.h"

namespace Wizzy {

	Application *Application::s_instance;

	Application::Application() {
	}

	Application::~Application() {
	}

	void Application::OnEvent(Event& e){
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


		WindowProps _windowProps = WindowProps(	_wndTitle,
												1600,
												900 );

		m_window = std::unique_ptr<IWindow>(IWindow::Create(_windowProps));
		m_window->SetEventCallback(WZ_BIND_FN(Application::OnEvent));

		m_imguiLayer = new ImguiLayer();
		PushOverlay(m_imguiLayer);

		m_running = true;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		float _vertices[] = {

			// front
			-2, -2, -2,
			0, 2, 0,
			2, -2, -2,

			// left
			-2, -2, 2,
			0, 2, 0,
			-2, -2, -2,

			// right
			2, -2, -2,
			0, 2, 0,
			2, -2, 2,

			// back
			2, -2, 2,
			0, 2, 0,
			-2, -2, 2,

			// bot topleft
			2, -2, 2,
			-2, -2, 2,
			-2, -2, -2,

			// bot botright
			-2, -2, -2,
			2, -2, -2,
			2, -2, 2

		};

		u32 _vbIdx;
		glGenBuffers(1, &_vbIdx);

		glBindBuffer(GL_ARRAY_BUFFER, _vbIdx);
		glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		Shader _shader("basic.vert", "basic.frag");

		_shader.Use();

		vec3 _camPos(0, 0, -10);

		vec3 _rot(0, 0, 0);
		vec3 _pos(0, 0, 0);
		vec3 _scale(1, 1, 1);

		auto _err = glGetError();

		WZ_CORE_ASSERT(_err == GL_NO_ERROR, "Error setting projection matrix");

		while (m_running) {
			m_window->OnFrameBegin();

			m_layerStack.UpdateLayers();

			auto _proj = glm::translate(glm::perspectiveFov<float>(90, 16, 9, .1f, 10000), _camPos);
			_shader.SetUniformMat4("projection", _proj);

			auto _view = glm::translate(glm::identity<mat4>(), _pos);
			_view = glm::rotate(_view, _rot.x, vec3(1, 0, 0));
			_view = glm::rotate(_view, _rot.y, vec3(0, 1, 0));
			_view = glm::rotate(_view, _rot.z, vec3(0, 0, 1));
			_view = glm::scale(_view, _scale);

			_shader.SetUniformMat4("view", _view);

			glDrawArrays(GL_TRIANGLES, 0, 18);

			m_imguiLayer->Begin();

			ImGui::Begin("Camera");

			ImGui::DragFloat3("Cam position", glm::value_ptr(_camPos), .1f);

			ImGui::Text("Prism");
			ImGui::DragFloat3("position", glm::value_ptr(_pos), .1f);
			ImGui::DragFloat3("rotation", glm::value_ptr(_rot), .05f);
			ImGui::DragFloat3("scale", glm::value_ptr(_scale), .05f);

			ImGui::End();

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
