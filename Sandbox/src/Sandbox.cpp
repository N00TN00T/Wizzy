#include "spch.h"

#include <Wizzy.h>

class Sandbox
	: public Wizzy::Application {
public:
	Sandbox() {
		
	}

};

Wizzy::Application* Wizzy::CreateApplication() {
	
	return new Sandbox();
}