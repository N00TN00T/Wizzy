#pragma once

#include "Wizzy/Renderer/API.h"
#include "Wizzy/Resource/Resource.h"

namespace Wizzy {

	struct ShaderProgramSource {
		string vertSource;
		string fragSource;
	};

    class Shader
        : public Resource {
    public:
        Shader(const Flagset& flags)
			: Resource(flags, "Shader", WZ_EXTENSION_SHADER) {}
		virtual
		~Shader();

        virtual
		void Bind() const = 0;

		virtual
		void Unbind() const = 0;

        virtual
		void UploadMat4(const string& name, const mat4& value) = 0;

		virtual
		void UploadMat3(const string& name, const mat3& value) = 0;

		virtual
		void Upload1i(const string& name, const int32& value)  = 0;

		virtual
		void Upload2i(const string& name, const int32* value) = 0;

        virtual
		void Upload3i(const string& name, const int32* value) = 0;

        virtual
		void Upload4i(const string& name, const int32* value) = 0;

		virtual
		void Upload1f(const string& name, const float& value)  = 0;

		virtual
		void Upload2f(const string& name, const vec2& value)  = 0;

		virtual
		void Upload3f(const string& name, const vec3& value)  = 0;

		virtual
		void Upload4f(const string& name, const vec4& value)  = 0;

		void UploadData(const string& name, ShaderDataType type, void* data);

	public:
		static
		Shader* Create(const string& sourceFile, const string& data, const Flagset& flags);
    };
}
