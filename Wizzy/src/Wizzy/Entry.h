#pragma once
#include "Log.h"

#ifdef WZ_PLATFORM_WINDOWS

extern Wizzy::Application* Wizzy::CreateApplication();

int main(int argc, char** argv) {

	Wizzy::Log::Init();
	WZ_CORE_INFO("Initialized the logs");

	auto _app = Wizzy::CreateApplication();
	_app->Run();
	delete _app;

	return 0;
}

#endif