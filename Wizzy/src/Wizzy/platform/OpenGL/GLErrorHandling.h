#pragma once

#include "Wizzy/Application.h"
#include "Wizzy/WizzyExceptions.h"

#define GL_CALL(glAction)  \
::Wizzy::__GL_Error_Handling::__GL_Clear_Error_Log();\
glAction;\
{if (auto _err = glGetError())\
	throw ::Wizzy::OpenGLErrorException(::Wizzy::__GL_Error_Handling::__GL_Error_To_String(_err), _err, __LINE__, __FUNCTION__); }


namespace Wizzy {
	namespace __GL_Error_Handling {
		void __GL_Clear_Error_Log();
		string __GL_Error_To_String(u32 err);
	}
}
