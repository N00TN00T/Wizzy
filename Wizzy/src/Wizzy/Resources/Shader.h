#pragma once

#include "Wizzy/Resources/IResource.h"

namespace Wizzy {

	struct ShaderProgramSource {
		string vertSource;
		string fragSource;
	};

    class Shader
        : public IResource {
    public:
        Shader(const string& file)
			: IResource(file) {}
		virtual ~Shader();

		virtual void Load() override;
		virtual void Unload() override;
		virtual void Reload() override;
		virtual void Save() override;

        virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

        virtual void SetUniformMat4(const string& name, const mat4& value) = 0;
		virtual void SetUniform1i(const string& name, const int32& value)  = 0;
		virtual void SetUniform1f(const string& name, const float& value)  = 0;
		virtual void SetUniform2f(const string& name, const vec2& value)  = 0;
		virtual void SetUniform3f(const string& name, const vec3& value)  = 0;
		virtual void SetUniform4f(const string& name, const vec4& value)  = 0;

    protected:
		virtual void ParseShader(const string& file) = 0;
        virtual bool Compile() = 0;
		virtual void Delete() = 0;

	public:
		static Shader* Create(const string& file);

    private:
        static u32 s_currentShader;
    };
}
