#include "wzpch.h"

#include "Wizzy/Renderer/Material.h"
#include "Wizzy/Renderer/Shader.h"
#include "Wizzy/Renderer/API.h"

namespace Wizzy {
    Material::Material(Shader *shader)
        : shader(shader), albedo(Color::white), diffuseTexture(nullptr),
          diffuseColor(Color::white) {

    }

    void Material::Bind() {
        
        if (diffuseTexture) {
            diffuseTexture->Bind(diffuseTexture->GetId());
            WZ_CORE_ASSERT(diffuseTexture->GetId() <= MaxTextureSlot(), "The wizzy renderer currently only supports {0} textures (graphics-drivers defined max)", MaxTextureSlot());
        }
        shader->Upload4f("albedo", albedo.ToVec4());

        if (diffuseTexture) {
            shader->Upload1i("hasTexture", 1);
            shader->Upload1i("diffuseTexture", diffuseTexture->GetId()); // TODO: texture slots
        } else {
            shader->Upload1i("hasTexture", 0);
            shader->Upload4f("diffuseColor", diffuseColor.ToVec4());
        }
    }
}
