#include "spch.h"

#include <Wizzy.h>

namespace wz = ::Wizzy;

class Sandbox
	: public Wizzy::Application {
public:
	Sandbox() {
		
	}

};

extern "C" {
    void* create_application() {
        Wizzy::Log::Init(Wizzy::LOG_CLIENT); // TODO
        return (void*)new Sandbox();
    }
}
