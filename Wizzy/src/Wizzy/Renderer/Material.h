#pragma once

#include "Wizzy/Renderer/API.h"
#include "Wizzy/Resource/Resource.h"

namespace Wizzy {

    struct MaterialProperty {
        ShaderDataType type;
        void *data;
    };

    struct Material
        : public Resource {
    public:
        Material(const string& data, const Flagset& flags);
        Material(ShaderHandle handle);
        void Bind();

        virtual
        string Serialize() const override;

        inline
        void SetProperty(const string& key, ShaderDataType type, void* data) {
            m_properties[key] = MaterialProperty { type, data };
        }

        inline static
        Material* Create(const string& sourceFile,
                         const string& data,
                         const Flagset& flags) {
            return new Material(data, flags);
        }

    public:
        ShaderHandle shaderHandle;
        Color albedo;
        Color diffuseColor;
        TextureHandle diffuseMapHandle;

    private:
        bool Init(const string& data);

    private:
        std::map<string, MaterialProperty> m_properties;
    };
}
