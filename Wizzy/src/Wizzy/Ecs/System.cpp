#include "wzpch.h"

#include "Wizzy/Ecs/System.h"

namespace Wizzy {
	void System::AddComponentType(StaticCId type, ComponentFlags flags) {
		m_typeIds.push_back(type);
		m_flags.push_back(flags);
		/* Stay invalid until a non-optional component
			is added										*/
		if ((flags & component_flag_optional) == 0) {
			m_isValid = true;
		}
	}

	void System::AddAllComponentTypes(ComponentFlags flags) {
		m_processAllComponents = true;
	}

	void System::Subscribe(Wizzy::EventType eventId) {
		m_subscriptions.push_back(eventId);
	}

	void System::SubscribeAll() {
		m_subscribeAll = true;
	}

	bool System::IsSubscribed(Wizzy::EventType eventId) const {
		if (m_subscribeAll) return true;
		for (auto _id : m_subscriptions) if (_id == eventId) return true;
		return false;
	}
}
