#pragma once

#ifdef WZ_PLATFORM_WINDOWS

extern Wizzy::Application* Wizzy::CreateApplication();

int main(int argc, char** argv) {
	auto _app = Wizzy::CreateApplication();
	_app->Run();
	delete _app;

	return 0;
}

#endif