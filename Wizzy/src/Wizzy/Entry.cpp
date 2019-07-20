#include "wzpch.h"
#include "Entry.h"
#include "Log.h"
#include "Wizzy/Application.h"

extern Wizzy::Application* CreateApplication();

#ifdef __GNUC__
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>


void handle_crash(int sig) {
    void *array[10];
    size_t size;

    size = backtrace(array, 10);

    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}
#endif

int main(int argc, char** argv) {

#ifdef __GNUC__
    signal(SIGSEGV, handle_crash);
#endif

    srand(time(NULL));

	Wizzy::Log::Init();
    WZ_CORE_INFO("Initialized the logs");

    try {
        auto _app = CreateApplication();
    	_app->Run();
    	delete _app;
    } catch (std::exception& e) {
        WZ_CORE_CRITICAL("Unhandeled exception: {0}", e.what());
    }


	return 0;
}
