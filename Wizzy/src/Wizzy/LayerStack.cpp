#include <wzpch.h>
#include "LayerStack.h"

namespace Wizzy {
	LayerStack::LayerStack() {
		
	}
	LayerStack::~LayerStack() {
		for (Layer*& layer : m_layers) {
			layer->OnDetach();
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer * layer) {
		m_layers.emplace(m_layers.begin() + m_layerInsertIndex, layer);
		m_layerInsertIndex++;
		layer->OnAttach();
	}
	void LayerStack::PushOverlay(Layer * overlay) {
		m_layers.emplace_back(overlay);
		overlay->OnAttach();
	}
	void LayerStack::PopLayer(Layer * layer) {
		if (vector_contains(m_layers, layer)) {
			layer->OnDetach();
			m_layers.erase(std::find(m_layers.begin(), m_layers.end(), layer));
			m_layerInsertIndex--;
		}
	}
	void LayerStack::PopOverlay(Layer * overlay) {
		if (vector_contains(m_layers, overlay)) {
			overlay->OnDetach();
			m_layers.erase(std::find(m_layers.begin(), m_layers.end(), overlay));
		}
	}
	void LayerStack::UpdateLayers() {
		for (auto& layer : m_layers) {
			layer->OnUpdate();
		}
	}
	void LayerStack::OnEvent(Event & e) {
		for (int16 i = m_layers.size() - 1; i >= 0; i--) {
			m_layers.at(i)->OnEvent(e);

			if (e.IsHandled())
				break;
		}
	}
	void LayerStack::OnImguiRender() {
		for (auto& layer : m_layers) {
			layer->OnImguiRender();
		}
	}
}