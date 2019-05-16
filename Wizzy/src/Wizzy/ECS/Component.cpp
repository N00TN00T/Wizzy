#include "wzpch.h"

#include "Wizzy/ECS/Component.h"

namespace Wizzy {

	StaticCId IComponent::__REGISTER_COMPONENT_TYPE(ComponentCreateFn createFn,
													ComponentFreeFn freeFn,
													size_t size) {
		componentTypes().push_back({
			createFn,
			freeFn,
			size
		});

		return static_cast<StaticCId>(componentTypes().size() - 1);
	}
	const ComponentInfo & IComponent::StaticInfo(StaticCId componentType) {
		return componentTypes()[componentType];
	}
}
