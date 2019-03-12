#include <wzpch.h>
#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Wizzy {
	
	std::shared_ptr<spdlog::logger> Log::s_coreLogger;
	std::shared_ptr<spdlog::logger> Log::s_clientLogger;

	void Log::Init() {
		spdlog::set_pattern("[%H:%M:%S:%e] %n: %^(%l) %v%$");

		s_coreLogger = spdlog::stdout_color_mt("CORE");
		SetCoreLogLevel(LOG_LEVEL_TRACE);
		
		s_clientLogger = spdlog::stdout_color_mt("CLIENT");
		SetClientLogLevel(LOG_LEVEL_TRACE);
	}
}
