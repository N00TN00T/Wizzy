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