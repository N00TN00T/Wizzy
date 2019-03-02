#pragma once

#include "Core.h"
#include <spdlog\spdlog.h>
#include <memory>
#include <stdio.h>

namespace Wizzy {
	class WZ_API Log {
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_coreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_clientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_coreLogger;
		static std::shared_ptr<spdlog::logger> s_clientLogger;
	};
}

#if  defined(WZ_PLATFORM_WINDOWS) && defined(WZ_CONFIG_DEBUG)
	#define PRINT_PLACE printf("%s:%d:\n", __FILE__, __LINE__)
	#define PRINT_NEW_LINE printf("\n")
#else
	#define PRINT_PLACE
	#define PRINT_NEW_LINE 
#endif

#define WZ_CORE_TRACE(...) PRINT_PLACE;  ::Wizzy::Log::GetCoreLogger()->trace(__VA_ARGS__); PRINT_NEW_LINE
#define WZ_CORE_DEBUG(...) PRINT_PLACE; ::Wizzy::Log::GetCoreLogger()->debug(__VA_ARGS__); PRINT_NEW_LINE
#define WZ_CORE_INFO(...) PRINT_PLACE; ::Wizzy::Log::GetCoreLogger()->info(__VA_ARGS__); PRINT_NEW_LINE
#define WZ_CORE_WARN(...)  PRINT_PLACE; ::Wizzy::Log::GetCoreLogger()->warn(__VA_ARGS__); PRINT_NEW_LINE
#define WZ_CORE_ERROR(...)  PRINT_PLACE; ::Wizzy::Log::GetCoreLogger()->error(__VA_ARGS__); PRINT_NEW_LINE
#define WZ_CORE_CRITICAL(...)  PRINT_PLACE; ::Wizzy::Log::GetCoreLogger()->critical(__VA_ARGS__); PRINT_NEW_LINE

#define WZ_TRACE(...)  PRINT_PLACE; ::Wizzy::Log::GetClientLogger()->trace(__VA_ARGS__); PRINT_NEW_LINE
#define WZ_DEBUG(...) PRINT_PLACE; ::Wizzy::Log::GetClientLogger()->debug(__VA_ARGS__); PRINT_NEW_LINE
#define WZ_INFO(...)  PRINT_PLACE; ::Wizzy::Log::GetClientLogger()->info(__VA_ARGS__); PRINT_NEW_LINE
#define WZ_WARN(...)  PRINT_PLACE; ::Wizzy::Log::GetClientLogger()->warn(__VA_ARGS__); PRINT_NEW_LINE
#define WZ_ERROR(...)  PRINT_PLACE; ::Wizzy::Log::GetClientLogger()->error(__VA_ARGS__); PRINT_NEW_LINE
#define WZ_CRITICAL(...) PRINT_PLACE; ::Wizzy::Log::GetClientLogger()->critical(__VA_ARGS__); PRINT_NEW_LINE