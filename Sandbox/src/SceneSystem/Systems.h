#pragma once

#include "Components.h"

class SceneManager : public wz::System
{
public:
	SceneManager()
	{
		AddComponentType<SceneComponent>();

		Subscribe(wz::EventType::app_update);
	}

	virtual void OnEvent(const Wizzy::Event& e, wz::ComponentGroup& components) const override
	{
		auto& sceneComp = *components.Get<SceneComponent>();
		
		switch (e.GetEventType())
		{
			case wz::EventType::app_update:
			{
				ImGui::Begin("Scene");

				sceneComp.ShowGUI();

				ImGui::End();

				return;
			}
		}
	}
};