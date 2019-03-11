#include "spch.h"

#include <Wizzy.h>

class ExampleLayer
	: public Wizzy::Layer {
public:
	ExampleLayer()
		: Wizzy::Layer::Layer("ExampleLayer") {
	}
public:
	virtual void OnImguiRender() override {
		
	}

	virtual void OnUpdate() override {
		if (Wizzy::Input::GetKey(WZ_KEY_E)){
			WZ_DEBUG("E PRESSED BABYYYY");
		}
	}

};

class Sandbox
	: public Wizzy::Application {
public:
	Sandbox() {
		PushOverlay(new ExampleLayer());
	}

};

Wizzy::Application* Wizzy::CreateApplication() {
	
	return new Sandbox();
}