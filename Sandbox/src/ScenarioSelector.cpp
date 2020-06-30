#include "spch.h"
#include <Wizzy.h>

#define SANDBOX		    0
#define DEMOGAME        1
#define GENERAL         2
#define GAD180_BREAKOUT 3

#define SELECTION	GAD180_BREAKOUT

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
#elif SELECTION == GENERAL
#include "GeneralTest.h"
Wizzy::Application* CreateApplication()
{
    return new GeneralTest();
}
#elif SELECTION == GAD180_BREAKOUT
#include "GAD180Breakout.h"
Wizzy::Application* CreateApplication()
{
    return new GAD180Breakout();
}
#endif