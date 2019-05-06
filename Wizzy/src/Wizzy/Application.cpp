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

		stbi_set_flip_vertically_on_load(true);

		int32 _imageWidth, _imageHeight, _imageChannels;

		byte *_textureData = stbi_load("box.png",
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
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _imageWidth, _imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, _textureData));

		GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

		stbi_image_free(_textureData);

		float _vertices[] = {

			// Pos			// Color		// Normal		// Uv

			// close top left	0
			-2, 2, -2,		1, 1, 1, 1,		-1, 1, -1,		0, 1,

			// close top right	1
			2, 2, -2,		1, 1, 1, 1,		1, 1, -1,		1, 1,

			// close bot left	2
			-2, -2, -2, 	1, 1, 1, 1, 	-1, -1, -1, 	0, 0,

			// close bot right	3
			2, -2, -2, 		1, 1, 1, 1, 	1, -1, -1, 		1, 0,

			// far top left		4
			-2, 2, 2, 		1, 1, 1, 1, 	-1, 1, 1, 		1, 1,

			// far top right	5
			2, 2, 2, 		1, 1, 1, 1, 	1, 1, 1, 		0, 1,

			// far bot left		6
			-2, -2, 2, 		1, 1, 1, 1, 	-1, -1, 1, 		1, 0,

			// far bot right	7
			2, -2, 2, 		1, 1, 1, 1, 	1, -1, 1, 		0, 0,




			
			// close top left	8
			-2, 2, -2,		1, 1, 1, 1,		-1, 1, -1,		1, 1,

			// close top right	9
			2, 2, -2,		1, 1, 1, 1,		1, 1, -1,		0, 1,

			// close bot left	10
			-2, -2, -2, 	1, 1, 1, 1, 	-1, -1, -1, 	1, 1,

			// close bot right	11
			2, -2, -2, 		1, 1, 1, 1, 	1, -1, -1, 		0, 1,

			// far top left		12
			-2, 2, 2, 		1, 1, 1, 1, 	-1, 1, 1, 		1, 0,

			// far top right	13
			2, 2, 2, 		1, 1, 1, 1, 	1, 1, 1, 		0, 0,

			// far bot left		14
			-2, -2, 2, 		1, 1, 1, 1, 	-1, -1, 1, 		1, 0,

			// far bot right	15
			2, -2, 2, 		1, 1, 1, 1, 	1, -1, 1, 		0, 0,
		};

		u32 _indices[] = {
			// front
			0, 1, 3,
			3, 2, 0,

			// left
			6, 4, 0,
			0, 2, 6,

			// top
			13, 9, 8,
			8, 12, 13,

			// right
			7, 3, 1,
			1, 5, 7,

			// bot
			10, 11, 15,
			15, 14, 10,

			// back
			7, 5, 4,
			4, 6, 7,

		};

		VertexArray _vao;

		VertexBuffer _vbo(_vertices, (8 * 3 + 8 * 4 + 8 * 3 + 8 * 2) * 2 * sizeof(float));
		VertexBufferLayout _vboLayout;
		_vboLayout.Push<float>(3);
		_vboLayout.Push<float>(4);
		_vboLayout.Push<float>(3);
		_vboLayout.Push<float>(2);
		_vao.AddBuffer(_vbo, _vboLayout);

		IndexBuffer _ibo(_indices, 12 * 3);

		Material _material1;
		Material _material2;

		Light _light;

		Renderer _renderer;

		vec3 _camPos(0, 0, -10);

		struct Box {
			vec3 rot;
			vec3 pos;
			vec3 scale;

			mat4 Model() {
				auto _model = glm::translate(glm::identity<mat4>(), pos);
				_model = glm::scale(_model, scale);
				_model = glm::rotate(_model, rot.x, vec3(1, 0, 0));
				_model = glm::rotate(_model, rot.y, vec3(0, 1, 0));
				_model = glm::rotate(_model, rot.z, vec3(0, 0, 1));
				return _model;
			}
		};

		Box box1 { vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1) };
		Box box2 { vec3(5, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1) };

		while (m_running) {
			m_window->OnFrameBegin();

			m_layerStack.UpdateLayers();
			
			auto _proj = glm::perspectiveFov<float>(glm::radians(45.f), m_window->GetWidth(), m_window->GetHeight(), .1f, 10000);
			auto _view = glm::translate(glm::identity<mat4>(), _camPos);

			_renderer.Begin();

			_renderer.SubmitRaw(_vao, _ibo, _material1, _proj, _view, box1.Model(), _camPos);

			static bool _renderBox2 = true;
			if (_renderBox2)
				_renderer.SubmitRaw(_vao, _ibo, _material2, _proj, _view, box2.Model(), _camPos);

			GL_CALL(glBindTexture(GL_TEXTURE_2D, _texId));

			_renderer.End(_light);

			m_imguiLayer->Begin();

			ImGui::Begin("Camera");

			ImGui::DragFloat3("position##0", glm::value_ptr(_camPos), .1f);

			ImGui::Text("Box1");
			ImGui::DragFloat3("position##1", glm::value_ptr(box1.pos), .1f);
			ImGui::DragFloat3("rotation##1", glm::value_ptr(box1.rot), .05f);
			ImGui::DragFloat3("scale##1", glm::value_ptr(box1.scale), .05f);

			ImGui::Text("Material");
			static char _shader1[512] = "Basic Shader";
			if (ImGui::BeginCombo("Shader##1", _shader1)) {

				if (ImGui::MenuItem("Basic Shader")) {
					strcpy(_shader1, "Basic Shader");
				}

				if (ImGui::MenuItem("Basic Lighting Shader")) {
					strcpy(_shader1, "Basic Lighting Shader");
				}

				ImGui::EndCombo();
			}
			if (string(_shader1) == string("Basic Shader")) {
				_material1.SetShader(Shader::BasicShader());
			} else if (string(_shader1) == string("Basic Lighting Shader")) {
				_material1.SetShader(Shader::BasicLightingShader());
			}
			
			Color _albedo1 = _material1.GetAlbedo();
			Color _diffuse1 = _material1.GetDiffuse();
			Color _specular1 = _material1.GetSpecular();
			float _shininess1 = _material1.GetShininess();

			ImGui::ColorEdit4("albedo##1", _albedo1.rgba);
			ImGui::ColorEdit4("diffuse##1", _diffuse1.rgba);
			ImGui::ColorEdit4("Specular##1", _specular1.rgba);
			ImGui::SliderFloat("Shininess##1", &_shininess1, 0.01f, 256.f);

			_material1.SetAlbedo(_albedo1);
			_material1.SetDiffuse(_diffuse1);
			_material1.SetSpecular(_specular1);
			_material1.SetShininess(_shininess1);

			ImGui::Checkbox("Render Box2", &_renderBox2);

			if (_renderBox2) {
				ImGui::Text("Box2");
				ImGui::DragFloat3("position##2", glm::value_ptr(box2.pos), .1f);
				ImGui::DragFloat3("rotation##2", glm::value_ptr(box2.rot), .05f);
				ImGui::DragFloat3("scale##2", glm::value_ptr(box2.scale), .05f);

				ImGui::Text("Material");
				static char _shader2[512] = "Basic Shader";
				if (ImGui::BeginCombo("Shader##2", _shader2)) {

					if (ImGui::MenuItem("Basic Shader")) {
						strcpy(_shader2, "Basic Shader");
					}

					if (ImGui::MenuItem("Basic Lighting Shader")) {
						strcpy(_shader2, "Basic Lighting Shader");
					}

					ImGui::EndCombo();
				}
				if (string(_shader2) == string("Basic Shader")) {
					_material2.SetShader(Shader::BasicShader());
				} else if (string(_shader2) == string("Basic Lighting Shader")) {
					_material2.SetShader(Shader::BasicLightingShader());
				}

				Color _albedo2 = _material2.GetAlbedo();
				Color _diffuse2 = _material2.GetDiffuse();
				Color _specular2 = _material2.GetSpecular();
				float _shininess2 = _material2.GetShininess();

				ImGui::ColorEdit4("albedo##2", _albedo2.rgba);
				ImGui::ColorEdit4("diffuse##2", _diffuse2.rgba);
				ImGui::ColorEdit4("Specular##2", _specular2.rgba);
				ImGui::SliderFloat("Shininess##2", &_shininess2, 0.01f, 256.f);

				_material2.SetAlbedo(_albedo2);
				_material2.SetDiffuse(_diffuse2);
				_material2.SetSpecular(_specular2);
				_material2.SetShininess(_shininess2);
			}

			ImGui::Text("Light");
			ImGui::DragFloat3("Position##L", glm::value_ptr(_light.position), .1f);
			ImGui::ColorEdit3("Ambient##L", _light.ambient.rgba);
			ImGui::ColorEdit3("Diffuse##L", _light.diffuse.rgba);
			ImGui::ColorEdit3("Specular##L", _light.specular.rgba);
			ImGui::DragFloat("Intensity##L", &_light.intensity, .01f);
			ImGui::DragFloat("Range##L", &_light.range, .1f);

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
