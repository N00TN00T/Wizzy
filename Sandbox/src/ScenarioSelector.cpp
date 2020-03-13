#include "spch.h"

#define SANDBOX		    0
#define RENDERER2D	    1
#define NODESCRIPTING   2
#define PROJECTSYSTEM   3
#define SCENESYSTEM     4

#define SELECTION	SCENESYSTEM

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
#elif SELECTION == PROJECTSYSTEM
#include "ProjectSystem/ProjectSystem.h"
Wizzy::Application* CreateApplication()
{
    return new ProjectSystem();
}
#elif SELECTION == SCENESYSTEM
#include "SceneSystem/SceneSystem.h"
Wizzy::Application* CreateApplication()
{
    return new SceneSystem();
}
#endif