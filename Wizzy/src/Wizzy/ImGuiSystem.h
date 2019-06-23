#pragma once

namespace Wizzy {

	struct ImGuiComponent
		: public ecs::Component<ImGuiComponent> {
		ulib::Queue<std::function<void()>> imguiLayers;
	};

	class ImGuiSystem
		: public ecs::System {
	public:
		ImGuiSystem();
		~ImGuiSystem();
		virtual void OnEvent(const void* eventHandle,
							 ecs::ComponentGroup& components) const override;
	private:
		void InitImgui() const;
		void DeinitImgui() const;
		void ImguiBeginLayer(float delta) const;
		void OnRender(float delta, ImGuiComponent& imguiComponent) const;
		void ImguiEndLayer(float delta) const;
	};
}
