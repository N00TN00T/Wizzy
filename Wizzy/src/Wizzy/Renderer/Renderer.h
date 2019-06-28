#pragma once

#include "Wizzy/Renderer/RenderCommand.h"
#include "Wizzy/Renderer/Material.h"

namespace Wizzy {

    struct Submission {
        VertexArrayPtr va;
        Material material;
        mat4 transform;

        Submission(VertexArrayPtr va, Material material, mat4 transform)
            : va(va), material(material), transform(transform) {}
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
                    const mat4& transform);

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
        std::unordered_map<Shader*, std::deque<Submission>> s_submissions;
        static
        ulib::Queue<Shader*> s_shaderQueue;
    };
}
