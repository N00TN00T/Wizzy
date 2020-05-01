#include "spch.h"
#include <Wizzy.h>

#define SANDBOX		    0
#define DEMOGAME        1

#define SELECTION	DEMOGAME

#if SELECTION == SANDBOX
#include "Sandbox.h"
Wizzy::Application* CreateApplication()
{
    return new Sandbox();
}
#elif SELECTION == DEMOGAME
#include "DemoGame2D.h"
Wizzy::Application* CreateApplication()
{
    return new DemoGame2D();
}
#endif