#include "wzpch.h"

#include "Wizzy/Graphics/GLErrorHandling.h"
#include <glad/glad.h>
#include <GL/GLU.h>

namespace Wizzy {
	namespace __GL_Error_Handling {
		void __GL_Clear_Error_Log() { while (glGetError() != GL_NO_ERROR); }
		string __GL_Error_To_String(u32 err) {
			return (char*)gluErrorString(err);
		}
	}
}