#include "wzpch.h"

#include "Wizzy/Renderer/Material.h"
#include "Wizzy/Resource/ResourceManagement.h"

namespace Wizzy {
    Material::Material(ShaderHandle shaderHandle)
        : shaderHandle(shaderHandle),
          albedo(Color::white),
          diffuseTextureHandle(WZ_NULL_RESOURCE_HANDLE),
          diffuseColor(Color::white) {

    }

    void Material::Bind() {

        auto _shader = ResourceManagement::Get<Shader>(shaderHandle);

        _shader->Upload4f("albedo", albedo.ToVec4());

        if (!WZ_IS_RESOURCE_HANDLE_NULL(diffuseTextureHandle) && useTexture) {
            auto _diffuseTexture
                    = ResourceManagement::Get<Texture>(diffuseTextureHandle);
            _diffuseTexture->Bind(_diffuseTexture->GetId());
            WZ_CORE_ASSERT(_diffuseTexture->GetId() <= MaxTextureSlot(), "The wizzy renderer currently only supports {0} textures (graphics-drivers defined max)", MaxTextureSlot());
            _shader->Upload1i("hasTexture", 1);
            _shader->Upload1i("diffuseTexture", _diffuseTexture->GetId()); // TODO: texture slots
        } else {
            _shader->Upload1i("hasTexture", 0);
            _shader->Upload4f("diffuseColor", diffuseColor.ToVec4());
        }
    }
}
