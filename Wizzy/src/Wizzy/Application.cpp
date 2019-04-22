#include "wzpch.h"
#include "Application.h"
#include "Log.h"
#include "Events/AppEvent.h"
#include "Wizzy/Input.h"

#include <glad/glad.h>
#include <imgui.h>
#include <stb_image.h>

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

		//stbi_set_flip_vertically_on_load(true);

		int32 _imageWidth, _imageHeight, _imageChannels;

		byte *_textureData = stbi_load("box.jpeg",
										&_imageWidth,
										&_imageHeight,
										& _imageChannels,
										0);

		WZ_CORE_ASSERT(_textureData, "Failed loading texture");

		WZ_TRACE("W'{0}', H'{1}', C'{2}'", _imageWidth, _imageHeight, _imageChannels);

		u32 _texId;
		GL_CALL(glGenTextures(1, &_texId))

		GL_CALL(glBindTexture(GL_TEXTURE_2D, _texId));

		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));

		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _imageWidth, _imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, _textureData));

		GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

		stbi_image_free(_textureData);

		float _vertices[] = {

			// close top left
			-2, 2, -2,
			1, 1, 1, 1,
			-1, 1, -1,
			0, 1,

			// close top right
			2, 2, -2,
			1, 1, 1, 1,
			1, 1, -1,
			1, 1,

			// close bot left
			-2, -2, -2,
			1, 1, 1, 1,
			-1, -1, -1,
			0, 0,

			// close bot right
			2, -2, -2,
			1, 1, 1, 1,
			1, -1, -1,
			1, 0,

		};

		u32 _indices[] = {
			3, 1, 0,
			0, 2, 3,
		};

		VertexArray _vao;

		VertexBuffer _vbo(_vertices, (4 * 3 + 4 * 4 + 4 * 3 + 4 * 2) * sizeof(float));
		VertexBufferLayout _vboLayout;
		_vboLayout.Push<float>(3);
		_vboLayout.Push<float>(4);
		_vboLayout.Push<float>(3);
		_vboLayout.Push<float>(2);
		_vao.AddBuffer(_vbo, _vboLayout);

		IndexBuffer _ibo(_indices, 6);

		Material _material;

		Renderer _renderer;

		vec3 _camPos(0, 0, -10);

		struct Tri {
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

		Tri tri { vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1) };

		while (m_running) {
			m_window->OnFrameBegin();

			m_layerStack.UpdateLayers();

			auto _proj = glm::translate(glm::perspectiveFov<float>(90, 16, 9, .1f, 10000), _camPos);

			_renderer.Begin();

			_renderer.SubmitRaw(_vao, _ibo, _material, _proj * tri.View());
			//_renderer.SubmitRaw(_vao, _ibo, _material, _proj * glm::translate(glm::identity<mat4>(), vec3(3, 2, 0)));

			GL_CALL(glBindTexture(GL_TEXTURE_2D, _texId));

			_renderer.End();

			m_imguiLayer->Begin();

			ImGui::Begin("Camera");

			ImGui::DragFloat3("position##0", glm::value_ptr(_camPos), .1f);

			ImGui::Text("Triangle");
			ImGui::DragFloat3("position##1", glm::value_ptr(tri.pos), .1f);
			ImGui::DragFloat3("rotation##1", glm::value_ptr(tri.rot), .05f);
			ImGui::DragFloat3("scale##1", glm::value_ptr(tri.scale), .05f);

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
