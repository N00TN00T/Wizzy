#pragma once

#include "Wizzy/Renderer/API.h"
#include "Wizzy/Resource/Resource.h"
#include "Wizzy/Renderer/Shader.h"

namespace Wizzy {

	struct MaterialFieldData {
		ShaderDataType type;
		void *value;
	};

    struct Material : public Resource {
	public:
		__HANDLE_DEF;
    public:
        Material(const ResData& data, const PropertyLibrary& props);
        Material(Shader::Handle handle);
		~Material();
        void Bind();

        static const PropertyLibrary& GetTemplateProps();

        virtual
            ResData Serialize() const override;

        inline Shader::Handle GetShader() const { return m_hShader; }
        inline void SetShader(Shader::Handle h) { m_hShader = h; }

    private:
        Shader::Handle m_hShader;

    public:
        inline static
        Resource* Create(const ResData& data, const PropertyLibrary& props) {
            return new Material(data, props);
        }
	};
}
