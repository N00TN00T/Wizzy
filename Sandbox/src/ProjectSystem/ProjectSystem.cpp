#include "spch.h"

#include "ProjectSystem.h"

#include "../Renderer2DTest/Systems.h"

#include "ProjectManagement.h"

#include <portable-file-dialogs.h>

std::vector< wz::EntityHandle> entities;

struct TestComponent : public wz::Component<TestComponent>
{
	union
	{
		struct { int x, y, z; };
		int xyz[3];
	};
	char text[128] = "Big nut amirite";
	union
	{
		struct { float r, g, b; };
		float rgb[3];
	};

public: 
	TestComponent()
	{
		x = y = z = 0;
		r = g = b = 0;
	}
};

class TestSystem : public wz::System
{
public:
	TestSystem()
	{
		AddComponentType<TestComponent>();

		this->Subscribe(Wizzy::EventType::app_init);
		this->Subscribe(Wizzy::EventType::app_render);
	}

	virtual void OnEvent(const wz::Event& e, wz::ComponentGroup& components) const override
	{
		auto& comp = *components.Get<TestComponent>();
		switch (e.GetEventType())
		{
			case wz::EventType::app_init:
			{
				return;
			}
			case wz::EventType::app_render:
			{
				string nm = "Test#" + std::to_string(reinterpret_cast<uintptr_t>(comp.entity));
				ImGui::Begin(nm.c_str());				
				ImGui::DragInt3("xyz", comp.xyz);
				ImGui::DragFloat3("rgb", comp.rgb);
				ImGui::InputText("text", comp.text, 128);
				ImGui::End();
				return;
			}
		}
	}
};

struct ProjectComponent : public wz::Component<ProjectComponent>
{
	wz::SystemLayer projectSystems;
};

class ProjectManager : public wz::System
{
public:
	ProjectManager()
	{
		AddComponentType<ProjectComponent>();
		SubscribeAll();
	}

	virtual void OnEvent(const wz::Event& e, wz::ComponentGroup& components) const override
	{
		auto& comp = *components.Get<ProjectComponent>();

		if (e.GetEventType() == wz::EventType::app_init)
		{
			comp.projectSystems.AddSystem<TestSystem>();
		}

		if (ProjectManagement::GetProject() && ProjectManagement::GetProject()->valid)
		{
			ProjectManagement::GetEcs().NotifySystems(comp.projectSystems, e);
		}

		if (e.GetEventType() == wz::EventType::app_init)
		{
			wz::RenderCommand::Clear();
		}

		if (e.GetEventType() == wz::EventType::app_frame_begin)
		{
			wz::RenderCommand::Clear();
		}
		
		if (e.GetEventType() == wz::EventType::app_render)
		{

			ImGui::Begin("Tester");

			if (ProjectManagement::GetProject() && ImGui::Button("Add test"))
			{
				WZ_DEBUG("ADD TEST"); 
				ProjectManagement::GetEcs().CreateEntity<TestComponent>();
			}

			ImGui::End();

			ImGui::BeginMainMenuBar();
			auto barsz = ImGui::GetWindowSize();
			auto p = ProjectManagement::GetProject();
			ImGui::Text(ProjectManagement::GetProject() && p->valid ? p->name.c_str() : "[No project] ");

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::BeginMenu("New"))
				{
					static char name[128] = "";
					ImGui::InputText("Name", name, 128);
					static char directory[1024] = "";
					ImGui::InputText("Directory", directory, 1024);
					ImGui::SameLine();
					if (ImGui::Button("Browse"))
					{
						auto dir = pfd::select_folder("Select new project folder").result();
						strcpy(directory, dir.c_str());
					}
					
					if (ImGui::Button("Create"))
					{
						auto result = ProjectManagement::CreateProject(directory, name);

						if (result != ProjectManagement::CODE_OK)
						{
							WZ_ERROR("Error when creating project, code: {0}", result);
						}
						else
						{
							result = ProjectManagement::LoadProject(directory);
							if (result != ProjectManagement::CODE_OK)
							{
								WZ_ERROR("Error when loading project, code: {0}", result);
							}
						}

						strcpy(name, "");
						strcpy(directory, "");
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Load"))
				{
					auto path = pfd::open_file("Load Project File").result();

					if (path.size() > 0 && ProjectManagement::LoadProject(path[0]) != ProjectManagement::CODE_OK)
					{
						WZ_ERROR("Failed loading project");
					}
				}
				auto p = ProjectManagement::GetProject();
				if (ImGui::MenuItem("Save"))
				{
					if (ProjectManagement::SaveProject() != ProjectManagement::CODE_OK)
					{
						WZ_ERROR("Failed saving project");
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();

			ImGui::Begin(" ", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
			auto& wnd = wz::Application::Get().GetWindow();
			ImGui::SetWindowPos({ (float)wnd.GetPosX(), (float)wnd.GetPosY() + barsz.y });
			ImGui::SetWindowSize({ (float)wnd.GetWidth(), (float)wnd.GetHeight() - barsz.y });
			ImGui::End();

			

			
		}
	}
};

#define LOAD 0

void ProjectSystem::Init()
{
	wz::Log::SetCoreLogLevel(LOG_LEVEL_DEBUG);

	m_clientEcs.CreateEntity<ProjectComponent>();

	m_clientSystems.AddSystem<ProjectManager>();
}

void ProjectSystem::Shutdown()
{
	
}
