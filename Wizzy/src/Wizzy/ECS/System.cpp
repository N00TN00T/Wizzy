#include "wzpch.h"

#include "Wizzy/ECS/System.h"

namespace Wizzy {
	void System::AddComponentType(StaticCId type, ComponentFlags flags) {
		m_typeIds.push_back(type);
		m_flags.push_back(flags);
		/* Stay invalid until a non-optional component
			is added										*/
		if ((flags & FLAG_OPTIONAL) == 0) {
			m_isValid = true;
		}
	}
}