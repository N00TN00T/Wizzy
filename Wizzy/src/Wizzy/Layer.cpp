#include <wzpch.h>
#include "Layer.h"

namespace Wizzy {

#ifdef WZ_CONFIG_DEBUG
	Layer::Layer(const string & name) {
		m_name = name;
	}
	Layer::~Layer() {
	}
#endif

}