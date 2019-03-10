#pragma once

#include "Wizzy/Core.h"
#include "Wizzy/Layer.h"
#include "Wizzy/events/Event.h"

/*
	The ownership of the raw Layer pointers belong to the LayerStack as long
	as they are not manually popped.
*/

namespace Wizzy {

	class Layer;

	class WZ_API LayerStack {
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer *layer);
		void PushOverlay(Layer *overlay);
		void PopLayer(Layer *layer);
		void PopOverlay(Layer *overlay);

		void UpdateLayers();
		void OnEvent(Event& e);
		void OnImguiRender();

	private:
		std::vector<Layer*> m_layers;
		uint16 m_layerInsertIndex = 0;
	};

}