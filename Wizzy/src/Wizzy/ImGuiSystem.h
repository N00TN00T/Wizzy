#pragma once

#include "Wizzy/Ecs/Ecs.h"

namespace Wizzy {

	struct ImGuiComponent
		: public Component<ImGuiComponent> {

	};

	class ImGuiSystem : public System<ImGuiSystem, ImGuiComponent> {
	public:
		ImGuiSystem();
		~ImGuiSystem();
		bool ProcessComponents(const Event& e, ImGuiComponent* imguiComponent) const override;
	private:
		void Init() const;
		void Shutdown() const;
		void OnFrameBegin(float delta) const;
		void OnFrameEnd(float delta) const;
	};
}
