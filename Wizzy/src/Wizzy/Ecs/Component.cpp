#include "wzpch.h"

#include "Wizzy/Ecs/Component.h"

namespace Wizzy {

	const ComponentInfo & IComponent::StaticInfo(StaticCId componentType) {
		return componentTypes()[componentType];
	}
}
