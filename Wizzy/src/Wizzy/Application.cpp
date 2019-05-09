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

		ResourceManagement::SetResourcePath(BASE_DIR + string("Resources/"));

		ResourceManagement::Import<Texture2D> (
											"sprite.png",
											"sprite"
										);
		ResourceManagement::Import<Shader> (
											"basic.shader",
											"basicShader"
										);

		Shader& _shader = *ResourceManagement::Get<Shader>("basicShader");
		Texture2D& _tex2d = *ResourceManagement::Get<Texture2D>("sprite");

		float _vertices[] = {
		    // positions            // texture coords
		     0.5f,  0.5f, 0.0f,      1.0f, 1.0f,   // top right
		     0.5f, -0.5f, 0.0f,      1.0f, 0.0f,   // bottom right
		    -0.5f, -0.5f, 0.0f,      0.0f, 0.0f,   // bottom left
		    -0.5f,  0.5f, 0.0f,      0.0f, 1.0f    // top left
		};

		u32 _indices[] = {
		    0, 1, 3, // first triangle
		    1, 2, 3  // second triangle
    	};

		u32 VBO, VAO, EBO;
	    GL_CALL(glGenVertexArrays(1, &VAO));
	    GL_CALL(glGenBuffers(1, &VBO));
	    GL_CALL(glGenBuffers(1, &EBO));

	    GL_CALL(glBindVertexArray(VAO));

	    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_STATIC_DRAW));

	    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices), _indices, GL_STATIC_DRAW));

	    // position attribute
	    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
	    GL_CALL(glEnableVertexAttribArray(0));
	    // texture coord attribute
	    GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
	    GL_CALL(glEnableVertexAttribArray(2));

		_shader.Bind();

		_shader.SetUniform1i("ourTexture", 0);

		while (m_running) {
			m_window->OnFrameBegin();

			m_layerStack.UpdateLayers();

			_tex2d.Bind(GL_TEXTURE0);
			_shader.Bind();

			GL_CALL(glBindVertexArray(VAO));
        	GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

			if (Input::GetKey(WZ_KEY_R)) {
				_tex2d.Unbind();
				_shader.Unbind();
				_tex2d.Reload();
				_shader.Reload();
			}

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
