#pragma once

#include "Core.h"
#include "events/Event.h"

namespace Wizzy {

	class WZ_API Layer {
	public:
#ifdef WZ_CONFIG_DEBUG
		Layer(const string& name = "Layer");
#endif
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& e) {}

#ifdef WZ_CONFIG_DEBUG
		inline const string& GetName() const { return m_name; };

	private:
		string m_name;
#endif
	};

}