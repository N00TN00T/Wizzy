#pragma once

#include "Wizzy/Ecs/ECSManager.h"

namespace Wizzy {

	struct ImGuiComponent
		: public Wizzy::Component<ImGuiComponent> {

	};

	class ImGuiSystem
		: public Wizzy::System {
	public:
		ImGuiSystem();
		~ImGuiSystem();
		virtual void OnEvent(const Event& event,
							 Wizzy::ComponentGroup& components) const override;
	private:
		void Init() const;
		void Shutdown() const;
		void OnFrameBegin(float delta) const;
		void OnFrameEnd(float delta) const;
	};
}
