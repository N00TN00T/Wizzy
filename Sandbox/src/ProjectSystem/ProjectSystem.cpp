#include "spch.h"

#include "ProjectSystem.h"

#include "../Renderer2DTest/Systems.h"

#include "ProjectManagement.h"

#include <portable-file-dialogs.h>

struct ProjectComponent : public wz::Component<ProjectComponent>
{
	
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
		if (ProjectManagement::GetProject() && ProjectManagement::GetProject()->valid)
		{
			wz::SystemLayer systems;
			ProjectManagement::GetEcs().NotifySystems(systems, e);
		}

		if (e.GetEventType() == wz::EventType::app_frame_begin)
		{
			wz::RenderCommand::Clear();
		}
		
		if (e.GetEventType() == wz::EventType::app_render)
		{
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

			ImGui::Begin(" ", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
			auto& wnd = wz::Application::Get().GetWindow();
			ImGui::SetWindowPos({ (float)wnd.GetPosX(), (float)wnd.GetPosY() + barsz.y });
			ImGui::SetWindowSize({ (float)wnd.GetWidth(), (float)wnd.GetHeight() - barsz.y });
			ImGui::End();

			ImGui::Begin("hi");
			ImGui::Text("HEYY");
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
