#pragma once

#include "Wizzy/Core.h"

#include "Wizzy/Layer.h"

namespace Wizzy {
	class  ImguiLayer
		: public Layer {
	public:
		ImguiLayer();
		~ImguiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImguiRender() override;

		void Begin();
		void End();
	private:
	};
}