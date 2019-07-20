#include "wzpch.h"
#include "Application.h"
#include "Log.h"
#include "Events/AppEvent.h"
#include "Wizzy/Input.h"

#include <imgui.h>

#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include "Wizzy/ImGuiSystem.h"
#include "Wizzy/Renderer/API.h"
#include "Wizzy/Utils.h"
#include "Wizzy/Stopwatch.h"

#include "Wizzy/Resource/ResourceManagement.h"
#include "Wizzy/Renderer/Texture.h"
#include "Wizzy/Renderer/Model.h"
#include "Wizzy/Renderer/Material.h"
#include "Wizzy/Scripting/Script.h"

#define DISPATCH_EVENT_LOCAL(eType, ...) { auto _e = eType(__VA_ARGS__); this->OnEvent(_e); } [](){return 0;}()

namespace Wizzy {

	Application* Application::s_instance(nullptr);


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
		WZ_CORE_TRACE("Notifying client systems '{0}'", e);
		m_clientEcs.NotifySystems(m_clientSystems, _eHandle, (int32)e.GetEventType());
		WZ_CORE_TRACE("Notifying engine systems '{0}'", e);
		m_engineEcs.NotifySystems(m_engineSystems, _eHandle, (int32)e.GetEventType());
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

		ImGuiComponent _imguiComponent;
		ecs::IComponent* _comps[] = {
			&_imguiComponent
		};
		ecs::StaticCId _ids[] = {
			ImGuiComponent::staticId
		};

		m_engineEcs.CreateEntity(_comps, _ids, 1);

		m_engineSystems.AddSystem<ImGuiSystem>();

		this->Init();

		DISPATCH_EVENT_LOCAL(AppInitEvent);
		while (m_running) {
			m_window->OnFrameBegin();
			DISPATCH_EVENT_LOCAL(AppFrameBeginEvent, m_window->GetDeltaTime());

			DISPATCH_EVENT_LOCAL(AppUpdateEvent, m_window->GetDeltaTime());

			ImGui::Begin("Resource management");

			static char _path[1024] = "/home/";
			static char _handle[1024] = "ResourceHandle";
			static string _type = "Model";
			ImGui::InputText("Importpath", _path, 1024);
			ImGui::InputText("Handle", _handle, 1024);

			if (ImGui::BeginCombo("Import type", _type.c_str())) {

				if (ImGui::Selectable("Model")) {
					_type = "Model";
				}
				if (ImGui::Selectable("Texture")) {
					_type = "Texture";
				}
				if (ImGui::Selectable("Script")) {
					_type = "Script";
				}
				if (ImGui::Selectable("Material")) {
					_type = "Material";
				}

				ImGui::EndCombo();
			}

			if (ImGui::Button("Import")) {

				if (ulib::File::exists(_path)) {

					if (_type == "Model") {
						ResourceManagement::Load<Model>(_path, _handle);
					} else if (_type == "Texture") {
						ResourceManagement::Load<Texture>(_path, _handle);
					} else if (_type == "Script") {
						ResourceManagement::Load<Script>(_path, _handle);
					} else if (_type == "Material") {
						ResourceManagement::Load<Material>(_path, _handle);
					}

				}

			}

			ImGui::Spacing();

			if (ImGui::TreeNode("Resources")) {

				if (ImGui::TreeNode("Textures")) {

					for (auto& _handle : ResourceManagement::GetHandles()) {
						if (ResourceManagement::Is<Texture>(_handle)) {
							ImGui::TextUnformatted(_handle.c_str());
						}
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Models")) {

					for (auto& _handle : ResourceManagement::GetHandles()) {
						if (ResourceManagement::Is<Model>(_handle)) {
							ImGui::TextUnformatted(_handle.c_str());
						}
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Other")) {

					for (auto& _handle : ResourceManagement::GetHandles()) {
						if (!ResourceManagement::Is<Texture>(_handle) &&
							!ResourceManagement::Is<Model>(_handle)) {
							ImGui::TextUnformatted(_handle.c_str());
						}
					}

					ImGui::TreePop();
				}

				ImGui::TreePop();
			}

			ImGui::End();

			DISPATCH_EVENT_LOCAL(AppRenderEvent);

			DISPATCH_EVENT_LOCAL(AppFrameEndEvent, m_window->GetDeltaTime());

			m_window->OnFrameEnd();
		}

		this->Shutdown();
	}
}
