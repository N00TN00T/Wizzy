#pragma once

#include "Wizzy/Renderer/RenderCommand.h"
#include "Wizzy/Renderer/Material.h"

namespace Wizzy {

    class Renderer {
    public:
        static
        void Begin(const mat4& camTransform,
                    const mat4& projection); // env, lights, camera
        static
        void End();

        static
        void Submit(const VertexArrayPtr& va,
                    const Material& material,
                    const mat4& transform);

        inline static
        void SetAPI(int8 api) { RendererAPI::SetAPI(api); }
        inline static
        int8 GetAPI() { return RendererAPI::GetAPI(); }

    private:
        static mat4 s_camTransform;
        static mat4 s_projection;
        static bool s_isReady;
    };
}
