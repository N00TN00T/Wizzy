#pragma once

namespace Wizzy {
    class GLShader
        : public Shader {
    public:
        GLShader(const ResData& data, const PropertyTable& props);
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
        virtual void Upload1iv(const string& name, const u32& count, const int32* value) override;
        virtual void Upload2iv(const string& name, const u32& count, const int32* value) override;
        virtual void Upload3iv(const string& name, const u32& count, const int32* value) override;
        virtual void Upload4iv(const string& name, const u32& count, const int32* value) override;
        virtual void Upload1fv(const string& name, const u32& count, const float* value) override;
        virtual void Upload2fv(const string& name, const u32& count, const vec2& value) override;
        virtual void Upload3fv(const string& name, const u32& count, const vec3& value) override;
        virtual void Upload4fv(const string& name, const u32& count, const vec4& value) override;


        inline virtual ResData Serialize() const override {
            return ResData((const byte*)m_rawSource.data(), (const byte*)m_rawSource.data() + m_rawSource.length());
		}

    protected:
        bool ParseShader(const string& data);
        bool Compile();
    private:
        string              m_rawSource;
		ShaderProgramSource m_source;
        u32					m_shaderId;

        
    };
}
