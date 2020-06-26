#include "wzpch.h"

#include "Wizzy/Renderer/Material.h"
#include "Wizzy/Resource/ResourceManagement.h"
#include "Wizzy/Renderer/Shader.h"
#include "Wizzy/Renderer/Texture.h"
#include "Wizzy/Exceptions/WizzyExceptions.h"

namespace Wizzy {
    Material::Material(const ResData& data, const PropertyTable& props)
        : Resource(props), m_hShader(WZ_NULL_RESOURCE_HANDLE) 
    {
		
    }
    Material::Material(Shader::Handle shaderHandle)
        : Resource(PropertyTable()),
		  m_hShader(shaderHandle) {
		
		
    }

	Material::~Material() {
		
	}

    void Material::Bind() 
    {
		

        /*int32 _diffuseLocation = 0;
        int32 _specularLocation = 0;

        if (ResourceManagement::Is<Texture>(diffuseMapHandle)) {
            auto& _diffuseTexture = ResourceManagement::Get<Texture>(diffuseMapHandle);
            _diffuseLocation = _diffuseTexture.GetId();
            _diffuseTexture.Bind(_diffuseLocation);
            _shader.Upload1i("u_useDiffuseMap", 1);
        } else {
            _shader.Upload1i("u_useDiffuseMap", 0);
        }

        if (ResourceManagement::Is<Texture>(specularMapHandle)) {
            auto& _specularTexture = ResourceManagement::Get<Texture>(specularMapHandle);
            _specularLocation = _specularTexture.GetId();
            _specularTexture.Bind(_specularLocation);
            _shader.Upload1i("u_useSpecularMap", 1);
        } else {
            _shader.Upload1i("u_useSpecularMap", 0);
        }

        _shader.Upload4f("u_albedo", albedo.asVec4);

        if (!_shader.GetFlags().GetBit(WZ_SHADER_LIGHTING_MODE_NONE)) {
            if (_shader.GetFlags().GetBit(WZ_SHADER_LIGHTING_MODE_PHONG)) {
                _shader.Upload4f("u_diffuseColor", diffuseColor.asVec4);
                _shader.Upload1i("u_diffuseMap", _diffuseLocation);
                _shader.Upload1i("u_specularMap", _specularLocation);
                _shader.Upload4f("u_specularColor", specularColor.asVec4);
            } else if (_shader.GetFlags().GetBit(WZ_SHADER_LIGHTING_MODE_PBR)) {
                WZ_CORE_ASSERT(false, "PBR lighting not implemented");
            } else {
                WZ_CORE_ASSERT(false, "No lighting mode has been set in shader, something must have gone wrong at resource initialization");
            }

        }

        for (const auto& _prop : m_properties) {
            _shader.UploadData(_prop.first, _prop.second.type, _prop.second.data);
        }*/
    }

    ResData Material::Serialize() const {
        string _serialized = "shader:" + std::to_string(m_hShader.id) + "\n";

		// TODO: serialize data

		return ResData((byte*)_serialized.data(), ((byte*)_serialized.data()) + _serialized.size());
    }
    const PropertyTable& Material::GetTemplateProps()
    {
        return PropertyTable();
    }
}
