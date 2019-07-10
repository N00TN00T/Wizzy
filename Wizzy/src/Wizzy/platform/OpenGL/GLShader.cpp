#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/Renderer/Shader.h"
#include "Wizzy/platform/OpenGL/GLShader.h"
#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include "Wizzy/platform/OpenGL/GLAPI.h"

#define U_LOCATION(p, n) glGetUniformLocation(p, n)

namespace Wizzy {

    GLShader::GLShader(const string& data, const Flagset& flags)
        : m_rawSource(data), Shader(flags), m_shaderId(WZ_SHADER_ID_INVALID) {
        m_isValid = this->ParseShader(data) && this->Compile();
    }

    GLShader::~GLShader() {

    }

    void GLShader::Bind() const {
		GL_CALL(glUseProgram(m_shaderId));
    }

    void GLShader::Unbind() const {
        GL_CALL(glUseProgram(0));
    }

    void GLShader::UploadMat4(const string& name, const mat4& value) {
        GL_CALL(glUniformMatrix4fv(U_LOCATION(m_shaderId, name.c_str()), 1,
                                      GL_FALSE, glm::value_ptr(value)));
    }
    void GLShader::UploadMat3(const string& name, const mat3& value) {
        GL_CALL(glUniformMatrix3fv(U_LOCATION(m_shaderId, name.c_str()), 1,
                                      GL_FALSE, glm::value_ptr(value)));
    }

    void GLShader::Upload1i(const string & name, const int32 & value) {
		GL_CALL(glUniform1i(U_LOCATION(m_shaderId, name.c_str()), value));
	}

    void GLShader::Upload2i(const string & name, const int32 *value) {
		GL_CALL(glUniform2i(U_LOCATION(m_shaderId, name.c_str()), value[0], value[1]));
	}

    void GLShader::Upload3i(const string & name, const int32 *value) {
		GL_CALL(glUniform3i(U_LOCATION(m_shaderId, name.c_str()), value[0], value[1], value[2]));
	}

    void GLShader::Upload4i(const string & name, const int32 *value) {
		GL_CALL(glUniform4i(U_LOCATION(m_shaderId, name.c_str()), value[0], value[1], value[2], value[3]));
	}

	void GLShader::Upload1f(const string & name, const float & value) {
		GL_CALL(glUniform1f(U_LOCATION(m_shaderId, name.c_str()), value));
	}

	void GLShader::Upload2f(const string & name, const vec2 & value) {
		GL_CALL(glUniform2f(U_LOCATION(m_shaderId, name.c_str()), value.x, value.y));
	}

	void GLShader::Upload3f(const string& name, const vec3& value) {
		GL_CALL(glUniform3f(U_LOCATION(m_shaderId, name.c_str()), value.x, value.y, value.z));
	}

	void GLShader::Upload4f(const string& name, const vec4& value) {
		GL_CALL(glUniform4f(U_LOCATION(m_shaderId, name.c_str()), value.x, value.y, value.z, value.w));
	}

    bool GLShader::ParseShader(const string& data) {
        enum class ShaderType { invalid = -1, vertex = 0, fragment = 1 };
        enum ShaderLightMode  {
            none = WZ_SHADER_LIGHTING_MODE_NONE,
            phong = WZ_SHADER_LIGHTING_MODE_PHONG,
            pbr = WZ_SHADER_LIGHTING_MODE_PBR
        };

		WZ_CORE_TRACE("Parsing a shader program source from resource data");

		std::stringstream _sourceStream(data.c_str());

		string _vertSource = "";
		string _fragSource = "";
		ShaderType _shaderType = ShaderType::invalid;
        ShaderLightMode _lightMode = ShaderLightMode::none;

        u32 _lineNum = 1;
		string _line;
        while (std::getline(_sourceStream, _line, '\n')) {
            if (_line.find("#") != string::npos && _line.find("#shader") == string::npos && _line.find("#version") == string::npos && _shaderType != ShaderType::invalid) {
                WZ_CORE_ERROR("Failed parsing shader, settings must be set before specifying a shader with #shader token");
                WZ_CORE_TRACE(_line);
                return false;
            } else if (_line.find("#shader") != string::npos) {

                if (_line.find(" vertex") != string::npos) {
                    _shaderType = ShaderType::vertex;
                } else if (_line.find(" fragment") != string::npos) {
                    _shaderType = ShaderType::fragment;
                } else {
                    WZ_CORE_ERROR("Failed parsing shader, unknown #shader token");
                    return false;
                }

            } else if (_line.find("#lightmode") != string::npos) {

                if (_line.find(" none")) {
                    _lightMode = ShaderLightMode::none;
                } else if (_line.find(" phong")) {
                    _lightMode = ShaderLightMode::phong;
                } else if (_line.find(" pbr")) {
                    _lightMode = ShaderLightMode::pbr;
                } else {
                    WZ_CORE_ERROR("Failed parsing shader, unknown #lightmode token");
                    return false;
    			}

            } else {
                switch (_shaderType) {
                    case ShaderType::vertex:
                        _vertSource += _line + "\n";
                        break;
                    case ShaderType::fragment:
                        _fragSource += _line + "\n";
                        break;
                    case ShaderType::invalid:
                        if (_line != "\n") {
                            WZ_CORE_WARN("Shader source line '{0}' ignored as no shader type was specified (vertex/fragment)",
                                                            _lineNum);
                        }
                        break;
                }

            }
            _lineNum++;
        }

        m_flags.SetBit(_lightMode);

		m_source = { _vertSource, _fragSource };
        WZ_CORE_INFO("Successfully parsed shader");
    }

    bool GLShader::Compile() {
        WZ_CORE_TRACE("Compiling GL Shader...");
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

            if (!_vCompileSuccess){
                _errMsg += "Failed compiling vertex shader: ' "
                                                    + string(_vLog) + "'. ";
                delete _vLog;
            }
            if (!_fCompileSuccess) {
                _errMsg += "Failed compiling fragment shader: ' "
                                                    + string(_fLog) + "'";
                delete _fLog;
            }
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
        delete _linkLog;

        GL_CALL(glValidateProgram(_program));
        GL_CALL(glDeleteShader(_vShader));
        GL_CALL(glDeleteShader(_fShader));

        m_shaderId = _program;

        WZ_CORE_INFO("Successfully compiled GL Shader and assigned id '{0}'",
                                                                m_shaderId);

        return true;
    }
}
