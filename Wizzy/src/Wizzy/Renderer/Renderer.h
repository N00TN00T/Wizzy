#pragma once

#include "Wizzy/Renderer/API.h"
#include "Wizzy/Renderer/RenderCommand.h"
#include "Wizzy/Renderer/Material.h"

namespace Wizzy {

    struct Submission {
        VertexArrayPtr va;
        Material material;
        mat4 transform;
        RenderMode renderMode;

        Submission(VertexArrayPtr va, Material material, mat4 transform,
                   RenderMode renderMode)
            : va(va), material(material), transform(transform),
              renderMode(renderMode) {}
    };

    class Renderer {
    public:
        static
        void Begin(const mat4& camTransform); // env, lights, camera
        static
        void End();

        static
        void Submit(const VertexArrayPtr& va,
                    const Material& material,
                    const mat4& transform,
                    RenderMode mode = WZ_RENDER_MODE_TRIANGLES);

        inline static
        void SetAPI(int8 api) { RendererAPI::SetAPI(api); }
        inline static
        int8 GetAPI() { return RendererAPI::GetAPI(); }

    private:
        static
        void RenderSubmissions(std::deque<Submission>& submissions);

    private:
        static
        mat4 s_camTransform;
        static
        bool s_isReady;
        static
        std::unordered_map<ShaderHandle, std::deque<Submission>> s_submissions;
        static
        ulib::Queue<ShaderHandle> s_shaderQueue;
    };
}
