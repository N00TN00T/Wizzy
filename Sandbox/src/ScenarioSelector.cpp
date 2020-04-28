#include "spch.h"

#define SANDBOX		    0

#define SELECTION	SANDBOX

#if SELECTION == SANDBOX
#include "Sandbox.h"
Wizzy::Application* CreateApplication()
{
    return new Sandbox();
}
#endif