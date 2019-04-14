#include "wzpch.h"

#include "Wizzy/Resources/Shader.h"
#include "Wizzy/Utils.h"

#include <glad/glad.h>

#define U_LOCATION(p, n) glGetUniformLocation(p, n)

namespace Wizzy {
    u32 Shader::s_currentShader(-1);

    Shader::Shader(string vPath, string fPath)
        : m_vPath(vPath), m_fPath(fPath) {
        WZ_CORE_ASSERT(Load(), "Failed loading shader");
    }

    void Shader::Use() {
        if (s_currentShader != m_shaderId)
            glUseProgram(m_shaderId);
    }

    void Shader::SetUniformMat4(const string& name, const mat4& value) {
        glUniformMatrix4fv(U_LOCATION(m_shaderId, name.c_str()), 1,
                                      GL_FALSE, glm::value_ptr(value));
    }

    bool Shader::Load() {
#ifdef WZ_USE_OPENGL
        return __Load_OpenGL();
#else
    #error Wizzy currently only supports the OpenGL graphics API, make sure WZ_USE_OPENGL is defined
#endif
    }

    bool Shader::__Load_OpenGL() {
        u32 _program = glCreateProgram();
		u32 _vShader = glCreateShader(GL_VERTEX_SHADER);
		u32 _fShader = glCreateShader(GL_FRAGMENT_SHADER);

        string _vCodeStr;
        string _fCodeStr;

        bool _vertexReadSuccess = read_file(m_vPath.c_str(), &_vCodeStr);
        bool _fragReadSuccess = read_file(m_fPath.c_str(), &_fCodeStr);

        string _readErrMsg = "";
        if (!_vertexReadSuccess) _readErrMsg += "Failed reading vertex shader '"
                                               + m_vPath + "'. ";
        if (!_fragReadSuccess) _readErrMsg += "Failed reading fragment shader '"
                                              + m_fPath + "'.";
        WZ_CORE_ASSERT(_vertexReadSuccess && _fragReadSuccess, _readErrMsg);

        char *_vCode = new char[_vCodeStr.size()];
        char *_fCode = new char[_fCodeStr.size()];
        strcpy(_vCode, _vCodeStr.c_str());
        strcpy(_fCode, _fCodeStr.c_str());

        glShaderSource(_vShader, 1, &_vCode, NULL);
		glShaderSource(_fShader, 1, &_fCode, NULL);

		// COMPILE VERTEX SHADER
		glCompileShader(_vShader);
		int32 _vCompileResult;
		glGetShaderiv(_vShader, GL_COMPILE_STATUS, &_vCompileResult);

		// COMPILE FRAGMENT SHADER
		glCompileShader(_fShader);
		int32 _fCompileResult;
		glGetShaderiv(_fShader, GL_COMPILE_STATUS, &_fCompileResult);

        bool _vCompileSuccess = _vCompileResult != GL_FALSE;
        bool _fCompileSuccess = _fCompileResult != GL_FALSE;

        string _errMsg = "";
        if (!_vCompileSuccess || !_fCompileSuccess) {
            int32 _vLogLength;
            char *_vLog;
            if (!_vCompileSuccess) {
                glGetShaderiv(_vShader, GL_INFO_LOG_LENGTH, &_vLogLength);
                _vLog = new char[_vLogLength];
                glGetShaderInfoLog(_vShader, _vLogLength, &_vLogLength, _vLog);
            }
            int32 _fLogLength;
            char *_fLog;
            if (!_fCompileSuccess) {
                glGetShaderiv(_fShader, GL_INFO_LOG_LENGTH, &_fLogLength);
                _fLog = new char[_fLogLength];
                glGetShaderInfoLog(_fShader, _fLogLength, &_fLogLength, _fLog);
            }

            if (!_vCompileSuccess) _errMsg += "Failed compiling vertex shader '"
                                            + m_vPath + "': '" + string(_vLog)
                                            + "'.";
            if (!_fCompileSuccess) _errMsg += "Failed compiling fragment shader '"
                                            + m_fPath + "': '" + string(_fLog)
                                            + "'.";
        }
        WZ_CORE_ASSERT(_vCompileSuccess && _fCompileSuccess, _errMsg);

        glAttachShader(_program, _vShader);
        glAttachShader(_program, _fShader);

        glLinkProgram(_program);
        glValidateProgram(_program);

        glDeleteShader(_vShader);
        glDeleteShader(_fShader);

        m_shaderId = _program;

        return true;
    }
}
