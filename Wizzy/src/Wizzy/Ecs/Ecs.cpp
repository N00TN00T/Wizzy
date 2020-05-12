#include "wzpch.h"

#include "Wizzy/Ecs/Ecs.h"
#include "Wizzy/Ecs/SystemLayout.h"

namespace Wizzy {
	
	bool EcsInstance::ProcessLayout(SystemLayer& layout, const Event& e)
	{
		bool handle = false;
		for (int i = 0; i < layout.Count(); i++)
		{
			if (!layout.IsEnabled(i)) continue;
			if (layout._Process(i, e, this))
			{
				handle = true;
				break;
			}
		}

		return handle;
	}

}
