#pragma once

#include "Wizzy/Application.h"

#define GL_CALL(glAction)  \
::Wizzy::__GL_Error_Handling::__GL_Clear_Error_Log();\
glAction;\
{if (auto _err = glGetError())\
	WZ_CORE_ASSERT(false, "<GL Error> (" + std::to_string(_err) + "): " + ::Wizzy::__GL_Error_Handling::__GL_Error_To_String(_err)); }

namespace Wizzy {
	namespace __GL_Error_Handling {
		void __GL_Clear_Error_Log();
		string __GL_Error_To_String(u32 err);
	}
}
