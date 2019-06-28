#pragma once

namespace Wizzy {

	struct ImGuiComponent
		: public ecs::Component<ImGuiComponent> {

	};

	class ImGuiSystem
		: public ecs::System {
	public:
		ImGuiSystem();
		~ImGuiSystem();
		virtual void OnEvent(const void* eventHandle,
							 ecs::ComponentGroup& components) const override;
	private:
		void Init() const;
		void Shutdown() const;
		void OnFrameBegin(float delta) const;
		void OnFrameEnd(float delta) const;
	};
}
