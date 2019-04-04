#include "wzpch.h"
#include "Entry.h"
#include "Log.h"
#include "Wizzy/Application.h"

int main(int argc, char** argv) {

	Wizzy::Log::Init();

	auto _app = Wizzy::CreateApplication();
	_app->Run();
	delete _app;

	return 0;
}