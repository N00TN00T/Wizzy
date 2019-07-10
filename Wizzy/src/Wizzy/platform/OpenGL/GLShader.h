#pragma once

namespace Wizzy {
    class GLShader
        : public Shader {
    public:
        GLShader(const string& data, const Flagset& flags);
        virtual ~GLShader();

        void Bind() const override;
		void Unbind() const override;

        virtual
        void UploadMat4(const string& name, const mat4& value) override;
        virtual
        void UploadMat3(const string& name, const mat3& value) override;
		virtual
        void Upload1i(const string& name, const int32& value) override;
        virtual
        void Upload2i(const string& name, const int32* value) override;
        virtual
        void Upload3i(const string& name, const int32* value) override;
        virtual
        void Upload4i(const string& name, const int32* value) override;
		virtual
        void Upload1f(const string& name, const float& value) override;
		virtual
        void Upload2f(const string& name, const vec2& value) override;
		virtual
        void Upload3f(const string& name, const vec3& value) override;
		virtual
        void Upload4f(const string& name, const vec4& value) override;

        inline virtual
        string Serialize() const override {
            return m_rawSource;
		}

    protected:
        bool ParseShader(const string& data);
        bool Compile();
    private:
		ShaderProgramSource m_source;
        u32					m_shaderId;
        string              m_rawSource;
    };
}
