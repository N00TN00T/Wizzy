#include <wzpch.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h> // TODO: Portability
#include <GLFW/glfw3.h>

#include "Wizzy/Core/ImGuiSystem.h"
#include "Wizzy/Core/Application.h"
#include "Wizzy/Events/AppEvent.h"

namespace Wizzy {
	ImGuiSystem::ImGuiSystem() {
		
		Subscribe(EventType::app_init);
		Subscribe(EventType::app_frame_begin);
		Subscribe(EventType::app_frame_end);
	}
	ImGuiSystem::~ImGuiSystem() {
		Shutdown();
	}
	bool ImGuiSystem::ProcessComponents(const Event& e, ImGuiComponent* imguiComponent) const 
	{
		
		switch (e.GetEventType()) 
		{
			case EventType::app_init:
			{
				Init();
				break;
			}
			case EventType::app_frame_begin:
			{
				const AppFrameBeginEvent& _updateEvent = (const AppFrameBeginEvent&)e;
				OnFrameBegin(_updateEvent.GetDeltaTime());
				break;
			}
			case EventType::app_frame_end:
			{
				const AppFrameEndEvent& _updateEvent = (const AppFrameEndEvent&)e;
				OnFrameEnd(_updateEvent.GetDeltaTime());
			}
			default: break;
		}

		return false;
	}

	void ImGuiSystem::Init() const {
		WZ_CORE_TRACE("Initializing imgui...");

		auto _versionResult = IMGUI_CHECKVERSION();
		WZ_CORE_ASSERT(_versionResult, "Failed initialing imgui when checking version");

		auto _context = ImGui::CreateContext();
		WZ_CORE_ASSERT(_context, "Failed creating imgui context");

		auto& _io = ImGui::GetIO();
		_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		_io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		auto& _style = ImGui::GetStyle();

		_style.WindowRounding = 0.f;
		_style.Colors[ImGuiCol_WindowBg].w = 1.0f;

		WZ_CORE_TRACE("Doing imgui glfw setup task");

		Application& _app = Application::Get();

		GLFWwindow* _window = static_cast<GLFWwindow*>(_app.GetWindow().GetNativeWindow());

		bool _glfwInitResult = ImGui_ImplGlfw_InitForOpenGL(_window, true);
		WZ_CORE_ASSERT(_glfwInitResult, "Failed to initialize ImGui for glfw");

		bool _openglInitresult = ImGui_ImplOpenGL3_Init("#version 410");
		WZ_CORE_ASSERT(_openglInitresult, "Failed to initialize ImGui for OpenGL3");

		WZ_CORE_INFO("Successfully initialized ImGui");
	}

	void ImGuiSystem::Shutdown() const {
		
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	void ImGuiSystem::OnFrameBegin(float delta) const {
		WZ_CORE_TRACE("Doing imgui frame setup task");
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}
	void ImGuiSystem::OnFrameEnd(float delta) const {
		WZ_CORE_TRACE("Ending imgui layer...");
		auto& _io = ImGui::GetIO();
		auto& _app = Application::Get();
		_io.DisplaySize = ImVec2((float)_app.GetWindow().GetWidth(),
			(float)_app.GetWindow().GetHeight());

		_io.DeltaTime = static_cast<float>(delta);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (_io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* _backupCurrentContext = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault(); // TODO: Portability (Works on linux for some reason)
			glfwMakeContextCurrent(_backupCurrentContext);
		}
	}
}
