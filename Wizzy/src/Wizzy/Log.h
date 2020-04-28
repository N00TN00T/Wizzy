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
    typedef int LogLevel;

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

    inline std::string LogLevelToString(LogLevel level) {
        switch (level) {
            case LOG_LEVEL_TRACE:       return "trace";
            case LOG_LEVEL_DEBUG:       return "debug";
            case LOG_LEVEL_INFO:        return "info";
            case LOG_LEVEL_WARN:        return "warn";
            case LOG_LEVEL_ERROR:       return "error";
            case LOG_LEVEL_CRITICAL:    return "critical";
        }

        return "invalid log level";
    }
}

#ifndef WZ_CONFIG_DIST

	#define WZ_CORE_TRACE(...)		{ SPDLOG_LOGGER_CALL(::Wizzy::Log::GetCoreLogger(), spdlog::level::trace, __VA_ARGS__); }
	#define WZ_CORE_DEBUG(...)		{ SPDLOG_LOGGER_CALL(::Wizzy::Log::GetCoreLogger(), spdlog::level::debug, __VA_ARGS__); }
	#define WZ_CORE_INFO(...)		{ SPDLOG_LOGGER_CALL(::Wizzy::Log::GetCoreLogger(), spdlog::level::info, __VA_ARGS__); }
	#define WZ_CORE_WARN(...)		{ SPDLOG_LOGGER_CALL(::Wizzy::Log::GetCoreLogger(), spdlog::level::warn, __VA_ARGS__); }
	#define WZ_CORE_ERROR(...)		{ SPDLOG_LOGGER_CALL(::Wizzy::Log::GetCoreLogger(), spdlog::level::err, __VA_ARGS__); }
	#define WZ_CORE_CRITICAL(...)	{ SPDLOG_LOGGER_CALL(::Wizzy::Log::GetCoreLogger(), spdlog::level::critical, __VA_ARGS__); }


	#define WZ_TRACE(...)		   { SPDLOG_LOGGER_CALL(::Wizzy::Log::GetClientLogger(), spdlog::level::trace, __VA_ARGS__); }
	#define WZ_DEBUG(...)		   { SPDLOG_LOGGER_CALL(::Wizzy::Log::GetClientLogger(), spdlog::level::debug, __VA_ARGS__); }
	#define WZ_INFO(...)		   { SPDLOG_LOGGER_CALL(::Wizzy::Log::GetClientLogger(), spdlog::level::info, __VA_ARGS__); }
	#define WZ_WARN(...)		   { SPDLOG_LOGGER_CALL(::Wizzy::Log::GetClientLogger(), spdlog::level::warn, __VA_ARGS__); }
	#define WZ_ERROR(...)		   { SPDLOG_LOGGER_CALL(::Wizzy::Log::GetClientLogger(), spdlog::level::err, __VA_ARGS__); }
	#define WZ_CRITICAL(...)	   { SPDLOG_LOGGER_CALL(::Wizzy::Log::GetClientLogger(), spdlog::level::critical, __VA_ARGS__); }

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
