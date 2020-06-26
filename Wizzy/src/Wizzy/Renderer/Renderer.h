#pragma once

#include "Wizzy/Renderer/API.h"
#include "Wizzy/Renderer/Buffers.h"
#include "Wizzy/Renderer/Material.h"
#include "Wizzy/Renderer/RenderTarget.h"
#include "Wizzy/Renderer/RenderCommand.h"

namespace Wizzy {

    class Renderer
    {
    private:
        struct Submission
        {
            const VertexArrayPtr& va;
            Material material;
            fmat4 transform;
        };
    public:
        
        static void Begin();

        static void Submit(const VertexArrayPtr& va, const Material& material, const fmat4& transform);

        static void Flush(const fmat4& cameraTransform = fmat4(1), RenderTargetPtr renderTarget = NULL);

        inline static bool IsBusy() { return s_isBusy; }

        inline static
        void SetAPI(int8 api) { RendererAPI::SetAPI(api); }
        inline static
        int8 GetAPI() { return RendererAPI::GetAPI(); }

    private:
        typedef u32 RenderTargetId;

        static RenderTargetPtr s_renderTarget;
        static std::queue<Shader::Handle> s_shaderQueue;
        static Resource::HandleMap<std::queue<Submission>> s_submissionQueue;
        static bool s_isBusy;
    };
}
