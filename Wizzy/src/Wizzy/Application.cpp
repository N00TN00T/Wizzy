#include "wzpch.h"
#include "Application.h"
#include "Log.h"
#include "Events/AppEvent.h"
#include "Wizzy/Input.h"

#include <glad/glad.h>
#include <imgui.h>

#include "Wizzy/Graphics/Material.h"
#include "Wizzy/Graphics/GLErrorHandling.h"
#include "Wizzy/Graphics/VertexArray.h"
#include "Wizzy/Graphics/IndexBuffer.h"
#include "Wizzy/Graphics/Renderer.h"

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

		float _vertices[] = {

			// posx, posy, posz
			// colr, colg, colb, cola,
			// nrmx, nrmy, nrmz

			// top
			0, 2, 0,			// 0
			1.0, 0.5, 0.6, 1.0,
			0.0, 1.0, 0.0,

			// bot close left
			-2, -2, -2,			// 1
			0.7, 0.2, 0.8, 1.0,
			-0.5, 0.0, -0.5,

			// bot close right
			2, -2, -2,			// 2
			0.1, 0.4, 0.2, 1.0,
			0.5, 0.0, -0.5,

			// bot far left
			-2, -2, 2,			// 3
			0.1, 0.1, 0.1, 1.0,
			-0.5, 0.0, 0.5,

			// bot far right
			2, -2, 2,			// 4
			0.4, 0.6, 0.3, 1.0,
			0.5, 0.0, 0.5,
		};

		u32 _indices[] = {

			// close
			0, 2, 1,

			// left
			0, 1, 3,

			// far
			0, 3, 4,

			// right
			0, 4, 2,

			// bot top-left
			1, 4, 3,

			// bot bot-right
			4, 1, 2,

		};

		VertexArray _vao;

		VertexBuffer _vbo(_vertices, (5 * 3 + 5 * 4 + 5 * 3) * sizeof(float));
		VertexBufferLayout _vboLayout;
		_vboLayout.Push<float>(3);
		_vboLayout.Push<float>(4);
		_vboLayout.Push<float>(3);
		_vao.AddBuffer(_vbo, _vboLayout);

		IndexBuffer _ibo(_indices, 6 * 3);

		Material _material;

		Renderer _renderer;

		vec3 _camPos(0, 0, -10);

		struct Prism {
			vec3 rot;
			vec3 pos;
			vec3 scale;

			mat4 View() {
				auto _view = glm::translate(glm::identity<mat4>(), pos);
				_view = glm::rotate(_view, rot.x, vec3(1, 0, 0));
				_view = glm::rotate(_view, rot.y, vec3(0, 1, 0));
				_view = glm::rotate(_view, rot.z, vec3(0, 0, 1));
				_view = glm::scale(_view, scale);
				return _view;
			}
		};

		Prism prismA{ vec3(0, 0, 0), vec3(-4, 0, 0), vec3(1, 1, 1) };
		Prism prismB{ vec3(0, 0, 0), vec3(4, 0, 0), vec3(1, 1, 1) };

		while (m_running) {
			m_window->OnFrameBegin();

			m_layerStack.UpdateLayers();

			auto _proj = glm::translate(glm::perspectiveFov<float>(90, 16, 9, .1f, 10000), _camPos);

			_renderer.Begin();

			_renderer.SubmitRaw(_vao, _ibo, _material, _proj * prismA.View());
			_renderer.SubmitRaw(_vao, _ibo, _material, _proj * prismB.View());

			_renderer.End();

			m_imguiLayer->Begin();

			ImGui::Begin("Camera");

			ImGui::DragFloat3("position##0", glm::value_ptr(_camPos), .1f);

			ImGui::Text("Prism A");
			ImGui::DragFloat3("position##1", glm::value_ptr(prismA.pos), .1f);
			ImGui::DragFloat3("rotation##1", glm::value_ptr(prismA.rot), .05f);
			ImGui::DragFloat3("scale##1", glm::value_ptr(prismA.scale), .05f);

			ImGui::Text("Prism B");
			ImGui::DragFloat3("position##2", glm::value_ptr(prismB.pos), .1f);
			ImGui::DragFloat3("rotation##2", glm::value_ptr(prismB.rot), .05f);
			ImGui::DragFloat3("scale##2", glm::value_ptr(prismB.scale), .05f);

			ImGui::Text("Material");
			Color _albedo = _material.GetAlbedo();
			ImGui::ColorEdit4("albedo##1", _albedo.rgba);
			_material.SetAlbedo(_albedo);

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
