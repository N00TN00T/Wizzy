#pragma once

namespace Wizzy {
    class GLShader
        : public Shader {
    public:
        GLShader(const string& file)
            : Shader(file), m_shaderId(WZ_SHADER_ID_INVALID) {}
        virtual ~GLShader();

        void Bind() const override;
		void Unbind() const override;

        virtual void UploadMat4(const string& name, const mat4& value) override;
        virtual void UploadMat3(const string& name, const mat3& value) override;
		virtual void Upload1i(const string& name, const int32& value) override;
        virtual void Upload2i(const string& name, const int32* value) override;
        virtual void Upload3i(const string& name, const int32* value) override;
        virtual void Upload4i(const string& name, const int32* value) override;
		virtual void Upload1f(const string& name, const float& value) override;
		virtual void Upload2f(const string& name, const vec2& value) override;
		virtual void Upload3f(const string& name, const vec3& value) override;
		virtual void Upload4f(const string& name, const vec4& value) override;

        inline virtual bool IsGarbage() const override {
			return m_shaderId == WZ_SHADER_ID_INVALID;
		}

    protected:
		virtual void ParseShader(const string& file) override;
        virtual bool Compile() override;
        virtual void Delete() override;
    private:
		ShaderProgramSource m_source;
        u32					m_shaderId;
    };
}
