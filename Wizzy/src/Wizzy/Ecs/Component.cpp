#include "wzpch.h"

#include "Wizzy/Ecs/Component.h"

namespace ecs {

	const ComponentInfo & IComponent::StaticInfo(StaticCId componentType) {
		return componentTypes()[componentType];
	}
}
