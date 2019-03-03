#include <wzpch.h>
#include "Log.h"
#include <spdlog\sinks\stdout_color_sinks.h>

namespace Wizzy {
	
	std::shared_ptr<spdlog::logger> Log::s_coreLogger;
	std::shared_ptr<spdlog::logger> Log::s_clientLogger;

	void Log::Init() {
		spdlog::set_pattern("[%H:%M:%S:%e] %^(%l)%$ %n: %v");

		s_coreLogger = spdlog::stdout_color_mt("CORE");
		s_coreLogger->set_level(spdlog::level::trace);

		s_clientLogger = spdlog::stdout_color_mt("CLIENT");
		s_clientLogger->set_level(spdlog::level::trace);
	}
}
