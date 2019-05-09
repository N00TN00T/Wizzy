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
			: IResource(file), m_shaderId(WZ_SHADER_ID_INVALID) {}
		~Shader();

		virtual void Load() override;
		virtual void Unload() override;
		virtual void Reload() override;
		virtual void Save() override;
		inline virtual bool IsGarbage() const override {
			return m_shaderId == WZ_SHADER_ID_INVALID;
		}

        void Bind() const;
		void Unbind() const;

        void SetUniformMat4(const string& name, const mat4& value);
		void SetUniform1i(const string& name, const int32& value);
		void SetUniform1f(const string& name, const float& value);
		void SetUniform2f(const string& name, const vec2& value);
		void SetUniform3f(const string& name, const vec3& value);
		void SetUniform4f(const string& name, const vec4& value);

    private:
		ShaderProgramSource ParseShader(const string& file);

        bool        Compile();
        bool        __Compile_OpenGL();

    private:
		ShaderProgramSource m_source;
        u32					m_shaderId;

	public:
		static std::shared_ptr<Shader> BasicShader();
		static std::shared_ptr<Shader> BasicLightingShader();

    private:
        static u32 s_currentShader;
    };

	typedef std::shared_ptr<Shader> ShaderPtr;
}
