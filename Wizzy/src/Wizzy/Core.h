#pragma once

#ifdef WZ_PLATFORM_WINDOWS
	#ifdef WZ_BUILD_DLL
		#define WZ_API __declspec(dllexport)
	#else
		#define WZ_API __declspec(dllimport)
	#endif
#else
	#error Wizzy currently only support windows x64
#endif