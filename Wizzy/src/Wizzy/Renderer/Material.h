#pragma once

#include "Wizzy/Renderer/API.h"
#include "Wizzy/Renderer/Texture.h"
#include "Wizzy/Renderer/Shader.h"

namespace Wizzy {

    struct Material {
        struct ShaderField {
            string id;
            ShaderDataType type;
            void* data;
        };
    public:
        Material(ShaderHandle shaderHandle);
        void Bind();

        ShaderHandle shaderHandle;
        Color albedo;
        TextureHandle diffuseTextureHandle;
        Color diffuseColor;
        bool useTexture = false;
    };
}
