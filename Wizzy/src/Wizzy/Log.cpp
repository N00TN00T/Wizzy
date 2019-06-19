#include <wzpch.h>
#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Wizzy {
	
	LoggerPtr Log::s_coreLogger;
	LoggerPtr Log::s_clientLogger;

	void Log::Init(LogType logType) {
		spdlog::set_pattern("[%H:%M:%S:%e] %n: %^(%l) %v%$");

        if (logType == LOG_CORE && !s_coreLogger) {
            s_coreLogger = spdlog::stdout_color_mt("CORE");
            SetCoreLogLevel(LOG_LEVEL_TRACE);
        } else if (logType == LOG_CLIENT && !s_clientLogger) {
            s_clientLogger = spdlog::stdout_color_mt("CLIENT");
            SetClientLogLevel(LOG_LEVEL_TRACE);
        }
	}
}
