#pragma once

#include "Wizzy/Renderer/API.h"
#include "Wizzy/Renderer/Texture.h"

namespace Wizzy {

    class Shader;

    struct Material {
        struct ShaderField {
            string id;
            ShaderDataType type;
            void* data;
        };
    public:
        Material(Shader *shader);
        void Bind();

        Shader *shader;
        Color albedo;
        Texture *diffuseTexture;
        Color diffuseColor;
    };
}
