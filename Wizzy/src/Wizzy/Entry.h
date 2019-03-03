#pragma once

#include "Log.h"

extern Wizzy::Application* Wizzy::CreateApplication();

int main(int argc, char** argv) {

	Wizzy::Log::Init();

	auto _app = Wizzy::CreateApplication();
	_app->Run();
	delete _app;

	return 0;
}

/* Entry points will vary depending on platforms */

#ifdef WZ_PLATFORM_WINDOWS

#else
	#warning Wizzy does not fully support other platforms than windows at the moment
#endif