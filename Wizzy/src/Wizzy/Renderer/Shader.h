#pragma once

#include "Wizzy/Renderer/API.h"
#include "Wizzy/Resource/Resource.h"

namespace Wizzy {

	struct ShaderField {

		ShaderDataType type;
		string name;

	};

	struct ShaderProgramSource {
		string vertSource;
		string fragSource;
	};

    class Shader : public Resource 
	{
    public:
        Shader(const PropertyLibrary& props)
			: Resource(props) {}
		virtual
		~Shader();

        virtual
		void Bind() const = 0;

		virtual
		void Unbind() const = 0;

        virtual void UploadMat4(const string& name, const mat4& value) = 0;

		virtual void UploadMat3(const string& name, const mat3& value) = 0;

		virtual void Upload1i(const string& name, const int32& value)  = 0;

		virtual void Upload2i(const string& name, const int32* value) = 0;

        virtual void Upload3i(const string& name, const int32* value) = 0;

        virtual void Upload4i(const string& name, const int32* value) = 0;

		virtual void Upload1f(const string& name, const float& value)  = 0;

		virtual void Upload2f(const string& name, const vec2& value)  = 0;

		virtual void Upload3f(const string& name, const vec3& value)  = 0;

		virtual void Upload4f(const string& name, const vec4& value) = 0;

		virtual void Upload1iv(const string& name, const u32& count, const int32* value) = 0;

		virtual void Upload2iv(const string& name, const u32& count, const int32* value) = 0;

		virtual void Upload3iv(const string& name, const u32& count, const int32* value) = 0;

		virtual void Upload4iv(const string& name, const u32& count, const int32* value) = 0;

		virtual void Upload1fv(const string& name, const u32& count, const float* value) = 0;

		virtual void Upload2fv(const string& name, const u32& count, const vec2& value) = 0;

		virtual void Upload3fv(const string& name, const u32& count, const vec3& value) = 0;

		virtual void Upload4fv(const string& name, const u32& count, const vec4& value) = 0;

		void UploadData(const string& name, ShaderDataType type, void* data);

		inline
		const std::vector<ShaderField>& GetFields() const { return m_fields; }

	protected:
		std::vector<ShaderField> m_fields;

	public:
		static
		Resource* Create(const ResData& data, const PropertyLibrary& props);

		static const PropertyLibrary& GetTemplateProps();

	private:
		static const PropertyLibrary* s_templateProps;
	};
}
