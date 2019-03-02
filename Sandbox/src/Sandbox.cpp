#include <Wizzy.h>

class Sandbox
	: public Wizzy::Application {
	
};

Wizzy::Application* Wizzy::CreateApplication() {
	return new Sandbox();
}