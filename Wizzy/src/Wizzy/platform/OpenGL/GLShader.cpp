#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/Renderer/Shader.h"
#include "Wizzy/platform/OpenGL/GLShader.h"
#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include "Wizzy/platform/OpenGL/GLAPI.h"
#include "Wizzy/Utils.h"
#include "Wizzy/PropertyLibrary.h"
#include "Wizzy/WizzyExceptions.h"

#define U_LOCATION(p, n) glGetUniformLocation(p, n)

bool string_is_glsl_type(const string& str) {
	return str == "mat4" || str == "vec2" || str == "vec3" || str == "vec4" || str == "mat3" || str == "float" || str == "int" || str == "bool" || str == "sampler2d";
}

namespace Wizzy {

	ShaderDataType string_to_shader_type(const string& str) {
		
		WZ_CORE_ASSERT(string_is_glsl_type(str), "String was not a supported glsl type");

		if		(str == "mat3")	return ShaderDataType::MAT3;
		else if (str == "mat4") return ShaderDataType::MAT4;
		else if (str == "vec2") return ShaderDataType::FLOAT2;
		else if (str == "vec3") return ShaderDataType::FLOAT3;
		else if (str == "vec4") return ShaderDataType::FLOAT4;
		else if (str == "float") return ShaderDataType::FLOAT1;
		else if (str == "int") return ShaderDataType::INT1;
		else if (str == "bool") return ShaderDataType::BOOL;
		else if (str == "sampler2d") return ShaderDataType::TEXTURE2D;

	}

    GLShader::GLShader(const ResData& data, const PropertyLibrary& props)
        : Shader(props), m_shaderId(WZ_SHADER_ID_INVALID) {

        m_rawSource = string((const char*)data.data(), data.size());
        if (!this->ParseShader(m_rawSource))
        {
            WZ_THROW(Exception, "Failed parsing shader");
        }
        if (!this->Compile())
        {
            WZ_THROW(Exception, "Failed compiling shader");
        }

		
    }

    GLShader::~GLShader() {
        GL_CALL(glDeleteProgram(m_shaderId));
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

    void GLShader::Upload1iv(const string& name, const u32& count, const int32* value)
    {
        GL_CALL(glUniform1iv(U_LOCATION(m_shaderId, name.c_str()), count, value));
    }
    void GLShader::Upload2iv(const string& name, const u32& count, const int32* value)
    {
        GL_CALL(glUniform2iv(U_LOCATION(m_shaderId, name.c_str()), count, value));
    }
    void GLShader::Upload3iv(const string& name, const u32& count, const int32* value)
    {
        GL_CALL(glUniform3iv(U_LOCATION(m_shaderId, name.c_str()), count, value));
    }
    void GLShader::Upload4iv(const string& name, const u32& count, const int32* value)
    {
        GL_CALL(glUniform4iv(U_LOCATION(m_shaderId, name.c_str()), count, value));
    }
    void GLShader::Upload1fv(const string& name, const u32& count, const float* value)
    {
        GL_CALL(glUniform1fv(U_LOCATION(m_shaderId, name.c_str()), count, value));
    }
    void GLShader::Upload2fv(const string& name, const u32& count, const vec2& value)
    {
        GL_CALL(glUniform2fv(U_LOCATION(m_shaderId, name.c_str()), count, glm::value_ptr(value)));
    }
    void GLShader::Upload3fv(const string& name, const u32& count, const vec3& value)
    {
        GL_CALL(glUniform3fv(U_LOCATION(m_shaderId, name.c_str()), count, glm::value_ptr(value)));
    }
    void GLShader::Upload4fv(const string& name, const u32& count, const vec4& value)
    {
        GL_CALL(glUniform4fv(U_LOCATION(m_shaderId, name.c_str()), count, glm::value_ptr(value)));
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
		string _processed = "";

        u32 _lineNum = 1;
		string _line;
        while (std::getline(_sourceStream, _line, '\n')) {
            if (_line.find("#") != string::npos && _line.find("#shader") == string::npos && _line.find("#version") == string::npos && _line.find("#define") == string::npos && _shaderType != ShaderType::invalid) {
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
                if (_line.find(" none") != string::npos) {
                    _lightMode = ShaderLightMode::none;
                } else if (_line.find(" phong") != string::npos) {
                    _lightMode = ShaderLightMode::phong;
                } else if (_line.find(" pbr") != string::npos) {
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
                            WZ_CORE_WARN("Shader source line '{0}' ignored as no shader type was specified (vertex/fragment/geometry)",
                                                            _lineNum);
                        }
                        break;
                }

            }

			if (_line.find("#") == string::npos || (_line.find("#") != string::npos && _line.find("#") > _line.find("//"))) {
				_processed += _line;
			}

            _lineNum++;
        }

        m_props.SetProperty("LightMode", (int32)_lightMode);

		m_source = { _vertSource, _fragSource };

		_sourceStream = std::stringstream(_processed);
		_line = "";

		while (std::getline(_sourceStream, _line, ';')) {

			auto _words = split_string(_line, ' ');

			/*bool _skipLine = false;
			for (const auto& _word : _words) {

				if (_word[0] == '/' && _word[1] == '/') { _skipLine = true; break; }

			}

			if (_skipLine) continue;*/

			bool _canSize5 = _words.size() == 5 && _words[3] == "=";

			if ((_canSize5 || _words.size() == 3) && _words[0] == "uniform" && string_is_glsl_type(_words[1])) {
				m_fields.push_back( { string_to_shader_type(_words[1]), _words[2] } );
			}
		}

        WZ_CORE_INFO("Successfully parsed shader");
        return true;
    }

    bool GLShader::Compile() {
        WZ_CORE_TRACE("Compiling GL Shader...");
        u32 _program;
        u32 _vShader;
        u32 _fShader;
        GL_CALL(_program = glCreateProgram());
		GL_CALL(_vShader = glCreateShader(GL_VERTEX_SHADER));
		GL_CALL(_fShader = glCreateShader(GL_FRAGMENT_SHADER));

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
                _errMsg += "Failed compiling vertex shader:\n    "
                                                    + string(_vLog);
                delete _vLog;
            }
            if (!_fCompileSuccess) {
                if (!_vCompileSuccess) _errMsg += "\n";
                _errMsg += "Failed compiling fragment shader:\n    "
                                                    + string(_fLog);
                delete _fLog;
            }
        }

		if (!_vCompileSuccess && !_fCompileSuccess) {
			WZ_CORE_ERROR(_errMsg);
			return false;
		}

        GL_CALL(glAttachShader(_program, _vShader));
        GL_CALL(glAttachShader(_program, _fShader));
        GL_CALL(glLinkProgram(_program));

        int32 _linkResult;
        GL_CALL(glGetProgramiv(_program, GL_LINK_STATUS, &_linkResult));

        bool _linkSuccess = _linkResult != GL_FALSE;

        char *_linkLog = new char[512];
        GL_CALL(glGetProgramInfoLog(_program, 512, NULL, _linkLog));

		if (!_linkSuccess) {
			WZ_CORE_ERROR("Failed linking shaders into program...");
			WZ_CORE_ERROR("    ...{0}", string(_linkLog));
			return false;
		}

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
