#pragma once

#include "Core.h"

#define LOG_LEVEL_TRACE 	((::Wizzy::LogLevel)spdlog::level::trace)
#define LOG_LEVEL_DEBUG 	((::Wizzy::LogLevel)spdlog::level::debug)
#define LOG_LEVEL_INFO 		((::Wizzy::LogLevel)spdlog::level::info)
#define LOG_LEVEL_WARN 		((::Wizzy::LogLevel)spdlog::level::warn)
#define LOG_LEVEL_ERROR 	((::Wizzy::LogLevel)spdlog::level::err)
#define LOG_LEVEL_CRITICAL 	((::Wizzy::LogLevel)spdlog::level::critical)

namespace Wizzy {

    typedef std::shared_ptr<spdlog::logger> LoggerPtr;
    typedef char LogLevel;

	class Log {
	public:
		static void Init();

		inline static LoggerPtr& GetCoreLogger() { return s_coreLogger; }
		inline static LoggerPtr& GetClientLogger() { return s_clientLogger; }

		inline static void SetCoreLogLevel(LogLevel level) {
			s_coreLogger->set_level((spdlog::level::level_enum)level);
		}

		inline static void SetClientLogLevel(LogLevel level) {
			s_clientLogger->set_level((spdlog::level::level_enum)level);
		}

	private:
		static LoggerPtr s_coreLogger;
		static LoggerPtr s_clientLogger;
	};
}

#if defined WZ_CONFIG_DEBUG
	#define PRINT_PREFIX_CORE(l)\
		if(::Wizzy::Log::GetCoreLogger()->level() <= (::Wizzy::LogLevel)l)\
			printf("==========%s:%d==========\n", __FILENAME__, __LINE__)
	#define PRINT_PREFIX(l) \
		if(::Wizzy::Log::GetClientLogger()->level() <= (::Wizzy::LogLevel)l)\
			printf("==========%s:%d==========\n", __FILENAME__, __LINE__)
#else
	#define PRINT_PREFIX(l)
	#define PRINT_PREFIX_CORE(l)
#endif

#ifndef WZ_CONFIG_DIST

	#define WZ_CORE_TRACE(...) PRINT_PREFIX_CORE(LOG_LEVEL_TRACE); ::Wizzy::Log::GetCoreLogger()->trace(__VA_ARGS__)
	#define WZ_CORE_DEBUG(...) PRINT_PREFIX_CORE(LOG_LEVEL_DEBUG); ::Wizzy::Log::GetCoreLogger()->debug(__VA_ARGS__)
	#define WZ_CORE_INFO(...) PRINT_PREFIX_CORE(LOG_LEVEL_INFO); ::Wizzy::Log::GetCoreLogger()->info(__VA_ARGS__)
	#define WZ_CORE_WARN(...)  PRINT_PREFIX_CORE(LOG_LEVEL_WARN); ::Wizzy::Log::GetCoreLogger()->warn(__VA_ARGS__)
	#define WZ_CORE_ERROR(...)  PRINT_PREFIX_CORE(LOG_LEVEL_ERROR); ::Wizzy::Log::GetCoreLogger()->error(__VA_ARGS__)
	#define WZ_CORE_CRITICAL(...)  PRINT_PREFIX_CORE(LOG_LEVEL_CRITICAL); ::Wizzy::Log::GetCoreLogger()->critical(__VA_ARGS__)

	#define WZ_TRACE(...)  PRINT_PREFIX(LOG_LEVEL_TRACE); ::Wizzy::Log::GetClientLogger()->trace(__VA_ARGS__)
	#define WZ_DEBUG(...) PRINT_PREFIX(LOG_LEVEL_DEBUG); ::Wizzy::Log::GetClientLogger()->debug(__VA_ARGS__)
	#define WZ_INFO(...)  PRINT_PREFIX(LOG_LEVEL_INFO); ::Wizzy::Log::GetClientLogger()->info(__VA_ARGS__)
	#define WZ_WARN(...)  PRINT_PREFIX(LOG_LEVEL_WARN); ::Wizzy::Log::GetClientLogger()->warn(__VA_ARGS__)
	#define WZ_ERROR(...)  PRINT_PREFIX(LOG_LEVEL_ERROR); ::Wizzy::Log::GetClientLogger()->error(__VA_ARGS__)
	#define WZ_CRITICAL(...) PRINT_PREFIX(LOG_LEVEL_CRITICAL); ::Wizzy::Log::GetClientLogger()->critical(__VA_ARGS__)

#else // In distribution mode

	#define WZ_CORE_TRACE(...)
	#define WZ_CORE_DEBUG(...)
	#define WZ_CORE_INFO(...)
	#define WZ_CORE_WARN(...)
	#define WZ_CORE_ERROR(...)  ::Wizzy::Log::GetCoreLogger()->error(__VA_ARGS__)
	#define WZ_CORE_CRITICAL(...)  ::Wizzy::Log::GetCoreLogger()->critical(__VA_ARGS__)

	#define WZ_TRACE(...)
	#define WZ_DEBUG(...)
	#define WZ_INFO(...)
	#define WZ_WARN(...)
	#define WZ_ERROR(...)  ::Wizzy::Log::GetClientLogger()->error(__VA_ARGS__)
	#define WZ_CRITICAL(...) ::Wizzy::Log::GetClientLogger()->critical(__VA_ARGS__)

#endif
