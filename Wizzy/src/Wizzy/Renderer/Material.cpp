#include "wzpch.h"

#include "Wizzy/Renderer/Material.h"
#include "Wizzy/Resource/ResourceManagement.h"
#include "Wizzy/Renderer/Shader.h"
#include "Wizzy/Renderer/Texture.h"

namespace Wizzy {
    Material::Material(const string& data, const Flagset& flags)
        : Resource(flags, "Material", WZ_EXTENSION_MATERIAL),
          shaderHandle(WZ_NULL_RESOURCE_HANDLE) {
        m_isValid = Init(data);
    }
    Material::Material(ShaderHandle shaderHandle)
        : Resource(Flagset(), "Material", WZ_EXTENSION_MATERIAL),
          shaderHandle(shaderHandle) {
        m_isValid = true;
    }

    void Material::Bind() {

        auto _shader = ResourceManagement::Get<Shader>(shaderHandle);

        int32 _textureLocation = -1;
        if (ResourceManagement::Is<Texture>(diffuseMapHandle)) {
            auto _diffuseTexture = ResourceManagement::Get<Texture>(diffuseMapHandle);
            _textureLocation = _diffuseTexture->GetId();
            _diffuseTexture->Bind(_textureLocation);
            _shader->Upload1i("useDiffuseMap", true);
        } else {
            _shader->Upload1i("useDiffuseMap", false);
        }

        _shader->Upload4f("albedo", albedo.asVec4);
        _shader->Upload4f("diffuseColor", diffuseColor.asVec4);
        _shader->Upload1i("diffuseMap", _textureLocation);


        for (const auto& _prop : m_properties) {
            _shader->UploadData(_prop.first, _prop.second.type, _prop.second.data);
        }
    }

    string Material::Serialize() const {
        WZ_CORE_ERROR("Material serialization not yet implemented");
        return "";
    }

    bool Material::Init(const string& data) {
        return true;
    }
}
