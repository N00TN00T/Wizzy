#pragma once

#include "Wizzy.h"
#include "Scene.h"

struct SceneComponent : public wz::Component<SceneComponent>
{
	Scene::Handle hndCurrentScene;

	virtual void ShowGUI() 
	{
		string curr = "None";
		if (wz::ResourceManagement::IsValid(hndCurrentScene))
		{
			curr = wz::ResourceManagement::GetInfoFor(hndCurrentScene).name();
		}

		if (ImGui::BeginCombo("Current Scene", curr.c_str()))
		{
			
			wz::ResourceManagement::ForEach<Scene>([&](wz::Resource::Handle hnd) 
			{
				string n = wz::ResourceManagement::GetInfoFor(hnd).name();
				if (ImGui::Selectable(n.c_str())) hndCurrentScene = (Scene::Handle)hnd;
			});
			ImGui::EndCombo();
		}
	}
};