#pragma once

#include "Core.h"
#include "Events/Event.h"

namespace Wizzy {

	class WZ_API Layer {
	public:
		Layer(const string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& e) {}
		virtual void OnImguiRender() {}

		inline const string& GetName() const { return m_name; };

	private:
		string m_name;
	};

}
