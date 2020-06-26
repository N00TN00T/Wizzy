#include "wzpch.h"
#include "Entry.h"
#include "Wizzy/Utilities/Log.h"
#include "Wizzy/Core/Application.h"
#include "Wizzy/Exceptions/WizzyExceptions.h"

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

int main(int argc, char** argv) 
{

    SetExecutablePath(argv[0]);

#ifdef __GNUC__
    signal(SIGSEGV, handle_crash);
#endif

    srand((unsigned int)time(NULL));
    
	Wizzy::Log::Init();
    WZ_CORE_INFO("Initialized the logs");

    try
    {
        auto _app = CreateApplication();
        _app->Run();
        delete _app;
    }
    catch (const std::exception & e)
    {
        WZ_CORE_CRITICAL("Unhandled std::exception: {0}", e.what());
        throw e;
    }
    catch (const Wizzy::Exception & e)
    {
        WZ_CORE_CRITICAL(e.GetUnhandledMessage());
        throw e;
    }

	return 0;
}
