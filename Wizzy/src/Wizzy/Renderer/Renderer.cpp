#include "wzpch.h"

#include "Wizzy/Renderer/Renderer.h"
#include "Wizzy/Renderer/Shader.h"
#include "Wizzy/Renderer/Texture.h"
#include "Wizzy/Resource/ResourceManagement.h"
#include "Wizzy/Utilities/Stopwatch.h"

namespace Wizzy {

    RenderTargetPtr Renderer::s_renderTarget;
    std::queue<Shader::Handle> Renderer::s_shaderQueue;
    Resource::HandleMap<std::queue<Renderer::Submission>> Renderer::s_submissionQueue;
    bool Renderer::s_isBusy;

    void Renderer::Begin()
    {
    }

    void Renderer::Submit(const VertexArrayPtr& va, const Material& material, const fmat4& transform)
    {
        WZ_CORE_ASSERT(va, "Vertex array cannot be null");

        s_shaderQueue.push(material.GetShader());
        s_submissionQueue[material.GetShader()].push({ va, material, transform });
    }

    void Renderer::Flush(const fmat4& cameraTransform, RenderTargetPtr renderTarget)
    {
        s_isBusy = true;
        if (renderTarget) renderTarget->Bind();

        while (!s_shaderQueue.empty())
        {
            auto hShader = s_shaderQueue.front();
            WZ_CORE_ASSERT(ResourceManagement::IsLoaded(hShader), "Shader is not loaded when flushing");

            Shader& shader = ResourceManagement::Get<Shader>(hShader);
            shader.Bind();

            shader.UploadMat4("u_camTransform", cameraTransform);

            auto& submissionQueue = s_submissionQueue[hShader];

            while (!submissionQueue.empty())
            {
                auto& submission = submissionQueue.front();

                shader.UploadMat4("u_transform", submission.transform);

                RenderCommand::DrawIndexed(submission.va, submission.va->GetIndexBuffer()->GetCount());

                submissionQueue.pop();
            }

            s_shaderQueue.pop();
        }

        if (renderTarget) renderTarget->Bind();

        s_isBusy = false;
    }
}