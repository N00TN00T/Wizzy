#include "wzpch.h"
#include "Entry.h"
#include "Log.h"
#include "Wizzy/Application.h"
#include <dlfcn.h>

namespace Wizzy {
	class Application;
}

#ifdef WZ_PLATFORM_WINDOWS

    #define load_lib(path)                  (LoadLibrary(TEXT(path)))
    #define load_symbol(lib, sym)           (GetProcAddress(lib, sym))
    #define close_lib(lib)                  (FreeLibrary(lib))
    #define lib_error                       ([](){ return "!Windows API doesn't have descriptions for these errors!" })()

    #define lib_dir_path                    ([](){ char *_path = new char[FILENAME_MAX]; GetModuleFileName( NULL, _path, FILENAME_MAX ); \
                                                    std::string _ret = ulib::File::directory_of(_path); delete _path; return _ret; })()
    #define lib_file_path                   (lib_dir_path + "/Sandbox.dll")
    #define lib_temp_file_path              (lib_dir_path + "/Sandbox_tmp.dll")

    #define CREATE_APPLICATION_SIGNATURE    void* __declspec(dllexport) create_application

#elif defined(WZ_PLATFORM_LINUX)
    #include <dlfcn.h>  // dlxxx functions
    #include <unistd.h> // readlink()

    #define load_lib(path)                  (dlmopen(LM_ID_NEWLM, path, RTLD_NOW | RTLD_DEEPBIND))
    #define load_symbol(lib, sym)           (dlsym(lib, sym))
    #define close_lib(lib)                  (dlclose(lib))
    #define lib_error                       (dlerror)

    #define lib_dir_path                    ([](){ char *_path = new char[FILENAME_MAX]; readlink( "/proc/self/exe", _path, FILENAME_MAX ); \
                                                    std::string _ret = ulib::File::directory_of(_path); delete _path; return _ret; })()
    #define lib_file_path                   (lib_dir_path + "/libSandbox.so")
	#define lib_temp_file_path              (lib_dir_path + "/libSandbox_tmp.so")

    #define CREATE_APPLICATION_SIGNATURE    void* create_application
#else
    #error Only windows and linux are supported at the moment
#endif

typedef void* (*CreateAppFn)();


int start_wizzy(int argc, char** argv) {

    srand(time(NULL));

	Wizzy::Log::Init(Wizzy::LOG_CORE);
    WZ_CORE_INFO("Initialized the logs!");

    bool _wantRestart = false;
    do {
		ulib::File::remove(lib_temp_file_path);
		WZ_CORE_ASSERT(ulib::File::copy(lib_file_path, lib_temp_file_path),
						"Failed making a temporary copy of game module");

		char *_path = new char[lib_temp_file_path.size()];
		strcpy(_path, lib_temp_file_path.c_str());

		WZ_CORE_TRACE("Loading game module from file '{0}'", ulib::File::to_absolute(_path));
		auto _lib = load_lib(_path);
		WZ_CORE_ASSERT(_lib != nullptr, "Failed loading game module: '" + string(lib_error()) + "'");
		if (!_lib) return 1;

		WZ_CORE_TRACE("Loading symbol 'create_application' from game module...");
		auto _createAppPtr = load_symbol(_lib, "create_application");
		WZ_CORE_TRACE("Casting function pointer into function...");
		CreateAppFn _createFn = (CreateAppFn)_createAppPtr;
		WZ_CORE_TRACE("Creating app with create function...");
		Wizzy::Application *_app = (Wizzy::Application*)_createFn();
		WZ_CORE_INFO("App created successfully from game module");
		_app->Run();
		_wantRestart = _app->WantRestart();
		delete _app;

		delete _path;
		ulib::File::remove(lib_temp_file_path);
		close_lib(_lib);

    } while (_wantRestart);



	return 0;
}
