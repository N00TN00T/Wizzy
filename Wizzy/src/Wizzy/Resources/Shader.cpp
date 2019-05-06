#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/Resources/Shader.h"
#include "Wizzy/Utils.h"
#include "Wizzy/Graphics/GLErrorHandling.h"

#define U_LOCATION(p, n) glGetUniformLocation(p, n)

namespace Wizzy {
    u32 Shader::s_currentShader(-1);

    Shader::Shader(const string& file) {
		m_source = ParseShader(file);
        WZ_CORE_ASSERT(Compile(), "Failed compiling shader");
    }

	Shader::Shader(const ShaderProgramSource & source)
		: m_source(source) {
		WZ_CORE_ASSERT(Compile(), "Failed compiling shader");
	}

    Shader::~Shader() {
        WZ_CORE_TRACE("Destructed shader");
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
		WZ_CORE_ASSERT(read_file(file, &_allSource), "Failed reading shader file '" + file + "'");

		std::stringstream _sourceStream(_allSource.c_str());

		string _vertSource;
		string _fragSource;
		ShaderType _shaderType = ShaderType::invalid;

		string _line;
		while (std::getline(_sourceStream, _line, '\n')) {
			if (_line.find("#shader") != std::string::npos) {
				if (_line.find("vertex") != std::string::npos)
					_shaderType = ShaderType::vertex;
				else if (_line.find("fragment") != std::string::npos)
					_shaderType = ShaderType::fragment;
			} else {
				switch (_shaderType) {
				case ShaderType::vertex:
					_vertSource += _line + "\n";
					break;
				case ShaderType::fragment:
					_fragSource += _line + "\n";
					break;
				}
			}
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
            char *_vLog;
            if (!_vCompileSuccess) {
                GL_CALL(glGetShaderiv(_vShader, GL_INFO_LOG_LENGTH, &_vLogLength));
                _vLog = new char[_vLogLength];
                GL_CALL(glGetShaderInfoLog(_vShader, _vLogLength, &_vLogLength, _vLog));
            }
            int32 _fLogLength;
            char *_fLog;
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

	std::shared_ptr<Shader> Shader::BasicShader() {
		static ShaderPtr _basicShader(std::make_shared<Shader>(Shader(ShaderProgramSource{
R"(

#version 410 core

layout (location = 0) in vec4 vertexPosition;
layout (location = 1) in vec4 vertexColor;
layout (location = 2) in vec4 vertexNormal;
layout (location = 3) in vec2 vertexUv;

uniform mat4 projection = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 model = mat4(1.0);

out DATA {
	vec4 color;
	vec2 uv;
} pass;

void main() {
	mat4 mvp = projection * view * model;
	pass.color = vertexColor;
	pass.uv = vertexUv;
	vec4 worldPosition = mvp * vertexPosition;
	gl_Position = worldPosition;
}

)",
R"(

#version 410 core

out vec4 finalColor;

uniform vec4 albedo = vec4(1.0, 1.0, 1.0, 1.0);
uniform sampler2D tex;

in DATA {
	vec4 color;
	vec2 uv;
} pass;

void main() {
	finalColor = texture(tex, pass.uv) * pass.color * albedo;
}

)" })));
		return _basicShader;
	}

	std::shared_ptr<Shader> Shader::BasicLightingShader() {
		static ShaderPtr _basicLightingShader(std::make_shared<Shader>(Shader(ShaderProgramSource{
	R"(

#version 410 core

layout (location = 0) in vec4 vertexPosition;
layout (location = 1) in vec4 vertexColor;
layout (location = 2) in vec3 vertexNormal;
layout (location = 3) in vec2 vertexUv;

uniform mat4 projection = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 model = mat4(1.0);

out DATA {
	vec4 color;
	vec3 normal;
	vec2 uv;
	vec3 fragPos;
} pass;

void main() {
	mat4 mvp = projection * view * model;
	pass.color = vertexColor;
	pass.uv = vertexUv;
	pass.normal = mat3(transpose(inverse(model))) * vertexNormal;
	pass.fragPos = (model * vertexPosition).xyz;
	vec4 worldPosition = mvp * vertexPosition;
	gl_Position = worldPosition;
}

)",
R"(

#version 410 core

out vec4 finalColor;

struct Material {
	vec4 albedo;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float intensity;
	float range;
};	

uniform Material material;
uniform Light light;

uniform vec3 viewPos = vec3(0.0, 0.0, 0.0);
uniform sampler2D tex;

in DATA {
	vec4 color;
	vec3 normal;
	vec2 uv;
	vec3 fragPos;
} pass;

void main() {

	vec3 lightDiff = light.position - pass.fragPos;

	float lightDist = length(lightDiff);

	vec3 norm = normalize(pass.normal);
	vec3 lightDir = normalize(lightDiff);
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse * material.diffuse;

	vec3 ambient = light.ambient;

	vec3 viewDir = normalize(viewPos - pass.fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = material.specular * light.specular * spec;

	float strength = light.intensity * ((light.range - lightDist) / light.range);
	if (strength < 0) strength = 0;

	vec4 lightResult = vec4((ambient + diffuse + specular) * strength, 1.0) * pass.color;
	finalColor = texture(tex, pass.uv) * lightResult * material.albedo;
}

)" })));
		return _basicLightingShader;
	}
}
