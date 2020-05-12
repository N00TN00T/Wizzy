#include "wzpch.h"

#include "Wizzy/Ecs/Component.h"

namespace Wizzy {

	const ComponentInfo & IComponent::StaticInfo(ComponentId componentType) {
		return componentTypes()[componentType - 1];
	}
}
