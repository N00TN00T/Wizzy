#include "spch.h"

#define SANDBOX		    0
#define RENDERER2D	    1
#define NODESCRIPTING   2

#define SELECTION	RENDERER2D

#if SELECTION == SANDBOX
#include "Sandbox.h"
Wizzy::Application* CreateApplication()
{
    return new Sandbox();
}
#elif SELECTION == RENDERER2D
#include "Renderer2Dtest/Renderer2DTest.h"
Wizzy::Application* CreateApplication()
{
    return new Renderer2DTest();
}
#elif SELECTION == NODESCRIPTING
#include "NodeScripting/NodeScripting.h"
Wizzy::Application* CreateApplication()
{
    return new NodeScripting();
}
#endif