#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/Resources/Shader.h"
#include "Wizzy/Utils.h"
#include "Wizzy/platform/OpenGL/GLErrorHandling.h"

#define U_LOCATION(p, n) glGetUniformLocation(p, n)

namespace Wizzy {

    Shader::~Shader() {
        if (!this->IsGarbage()) this->Unload();
    }

    void Shader::Load() {
        m_source = ParseShader(this->GetSourceFile());
        WZ_CORE_ASSERT(this->Compile(), "Failed compiling shader source");
        WZ_CORE_ASSERT(m_source.vertSource != "" && m_source.fragSource != "",
                        "Could not parse source for at least a vertex and fragment shader");
        WZ_CORE_INFO("Successfully compiled Shader and assigned id '{0}'",
                                                                m_shaderId);
    }
    void Shader::Unload() {
        WZ_CORE_TRACE("Unloading shader with id '{0}'...", m_shaderId);
        WZ_CORE_ASSERT(!this->IsGarbage(), "Tried unloading a garbage-flagged Shader");
        GL_CALL(glDeleteProgram(m_shaderId));
        m_shaderId = WZ_SHADER_ID_INVALID;
    }
    void Shader::Reload() {
        WZ_CORE_TRACE("Reloading shader...");
        Unload();
        Load();
    }
    void Shader::Save() {
        
    }

	void Shader::Bind() const {
		GL_CALL(glUseProgram(m_shaderId));
    }

    void Shader::Unbind() const {
        GL_CALL(glUseProgram(0));
    }

    void Shader::SetUniformMat4(const string& name, const mat4& value) {
        GL_CALL(glUniformMatrix4fv(U_LOCATION(m_shaderId, name.c_str()), 1,
                                      GL_FALSE, glm::value_ptr(value)));
    }

    void Shader::SetUniform1i(const string & name, const int32 & value) {
		GL_CALL(glUniform1i(U_LOCATION(m_shaderId, name.c_str()), value));
	}

	void Shader::SetUniform1f(const string & name, const float & value) {
		GL_CALL(glUniform1f(U_LOCATION(m_shaderId, name.c_str()), value));
	}

	void Shader::SetUniform2f(const string & name, const vec2 & value) {
		GL_CALL(glUniform2f(U_LOCATION(m_shaderId, name.c_str()), value.x, value.y));
	}

	void Shader::SetUniform3f(const string& name, const vec3& value) {
		GL_CALL(glUniform3f(U_LOCATION(m_shaderId, name.c_str()), value.x, value.y, value.z));
	}

	void Shader::SetUniform4f(const string& name, const vec4& value) {
		GL_CALL(glUniform4f(U_LOCATION(m_shaderId, name.c_str()), value.x, value.y, value.z, value.w));
	}

	ShaderProgramSource Shader::ParseShader(const string & file) {
        
		enum class ShaderType { invalid = -1, vertex = 0, fragment = 1 };

		WZ_CORE_TRACE("Parsing a shader program source from '{0}'", file);

		string _allSource;
		WZ_CORE_ASSERT(File::read(file, &_allSource), "Failed reading shader file '" + file + "'");

		std::stringstream _sourceStream(_allSource.c_str());

		string _vertSource = "";
		string _fragSource = "";
		ShaderType _shaderType = ShaderType::invalid;

        u32 _lineNum = 1;
		string _line;
		while (std::getline(_sourceStream, _line, '\n')) {
			if (_line.find("#shader") != std::string::npos) {
				if (_line.find("vertex") != std::string::npos)
					_shaderType = ShaderType::vertex;
				else if (_line.find("fragment") != std::string::npos)
					_shaderType = ShaderType::fragment;
                else
                    WZ_CORE_ASSERT(false, "Unknown #shader token in shader file '"
                                            + GetSourceFile() + "' on line "
                                            + std::to_string(_lineNum));
			} else {
				switch (_shaderType) {
				case ShaderType::vertex:
					_vertSource += _line + "\n";
					break;
				case ShaderType::fragment:
					_fragSource += _line + "\n";
					break;
                case ShaderType::invalid:
                    WZ_CORE_WARN("Shader source line '{0}' ignored as no shader source type was declared",
                                                    _lineNum);
                    break;
				}
			}
            _lineNum++;
		}

		return { _vertSource, _fragSource };
	}

	bool Shader::Compile() {
#ifdef WZ_USE_OPENGL
        return __Compile_OpenGL();
#else
    #error Wizzy currently only supports the OpenGL graphics API, make sure WZ_USE_OPENGL is defined
#endif
    }

    bool Shader::__Compile_OpenGL() {
		WZ_CORE_TRACE("Compiling OpenGL Shader...");
        GL_CALL(u32 _program = glCreateProgram());
		GL_CALL(u32 _vShader = glCreateShader(GL_VERTEX_SHADER));
		GL_CALL(u32 _fShader = glCreateShader(GL_FRAGMENT_SHADER));

		const char *_vSource = m_source.vertSource.c_str();
		const char *_fSource = m_source.fragSource.c_str();

        GL_CALL(glShaderSource(_vShader, 1, &_vSource, NULL));
		GL_CALL(glShaderSource(_fShader, 1, &_fSource, NULL));

		// COMPILE VERTEX SHADER
		GL_CALL(glCompileShader(_vShader));
		int32 _vCompileResult;
		GL_CALL(glGetShaderiv(_vShader, GL_COMPILE_STATUS, &_vCompileResult));

		// COMPILE FRAGMENT SHADER
		GL_CALL(glCompileShader(_fShader));
		int32 _fCompileResult;
		GL_CALL(glGetShaderiv(_fShader, GL_COMPILE_STATUS, &_fCompileResult));

        bool _vCompileSuccess = _vCompileResult != GL_FALSE;
        bool _fCompileSuccess = _fCompileResult != GL_FALSE;

        string _errMsg = "";
        if (!_vCompileSuccess || !_fCompileSuccess) {
            int32 _vLogLength;
            char *_vLog = NULL;
            if (!_vCompileSuccess) {
                GL_CALL(glGetShaderiv(_vShader, GL_INFO_LOG_LENGTH, &_vLogLength));
                _vLog = new char[_vLogLength];
                GL_CALL(glGetShaderInfoLog(_vShader, _vLogLength, &_vLogLength, _vLog));
            }
            int32 _fLogLength;
            char *_fLog = NULL;
            if (!_fCompileSuccess) {
                GL_CALL(glGetShaderiv(_fShader, GL_INFO_LOG_LENGTH, &_fLogLength));
                _fLog = new char[_fLogLength];
                GL_CALL(glGetShaderInfoLog(_fShader, _fLogLength, &_fLogLength, _fLog));
            }

            if (!_vCompileSuccess) _errMsg += "Failed compiling vertex shader: ' "
                                                + string(_vLog) + "'. ";
            if (!_fCompileSuccess) _errMsg += "Failed compiling fragment shader: ' "
                                                + string(_fLog) + "'";
        }
        WZ_CORE_ASSERT(_vCompileSuccess && _fCompileSuccess, _errMsg);

        GL_CALL(glAttachShader(_program, _vShader));
        GL_CALL(glAttachShader(_program, _fShader));
        GL_CALL(glLinkProgram(_program));

        int32 _linkResult;
        GL_CALL(glGetProgramiv(_program, GL_LINK_STATUS, &_linkResult));

        bool _linkSuccess = _linkResult != GL_FALSE;

        char *_linkLog = new char[512];
        GL_CALL(glGetProgramInfoLog(_program, 512, NULL, _linkLog));


        WZ_CORE_ASSERT(_linkSuccess, "Failed linking shaders into program: '"
                                        + string(_linkLog) + "'");

        GL_CALL(glValidateProgram(_program));
        GL_CALL(glDeleteShader(_vShader));
        GL_CALL(glDeleteShader(_fShader));

        m_shaderId = _program;

        return true;
    }
}
