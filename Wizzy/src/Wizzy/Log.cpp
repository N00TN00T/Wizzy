#include <wzpch.h>
#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/ostream_sink.h>

namespace Wizzy {

	LoggerPtr Log::s_coreLogger;
	LoggerPtr Log::s_clientLogger;

	void Log::Init() {
#ifdef WZ_CONFIG_DEBUG
		spdlog::set_pattern("%^==============================================================================\n[%n - %l - %H:%M:%S:%e - %s - Line %# - Thread %t]\n==============================================================================%$\n%v\n");
#elif defined(WZ_CONFIG_RELEASE)
		spdlog::set_pattern("%^%v%$\n");
#elif defined(WZ_CONFIG_DIST)
		spdlog::set_pattern("%v");
#else
#error No configuration defined
#endif
        s_coreLogger = spdlog::stdout_color_mt("Core");
        SetCoreLogLevel(LOG_LEVEL_TRACE);
		
        s_clientLogger = spdlog::stdout_color_mt("Client");
        SetClientLogLevel(LOG_LEVEL_TRACE);
	}
}
