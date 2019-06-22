#include "wzpch.h"
#include "Entry.h"
#include "Log.h"
#include "Wizzy/Application.h"
#include <dlfcn.h>

extern Wizzy::Application* CreateApplication();

int main(int argc, char** argv) {

    srand(time(NULL));

	Wizzy::Log::Init();
    WZ_CORE_INFO("Initialized the logs!");

    auto _app = CreateApplication();
	_app->Run();
	delete _app;

	return 0;
}
