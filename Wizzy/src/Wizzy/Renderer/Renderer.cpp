#include "wzpch.h"

#include "Wizzy/Renderer/Renderer.h"
#include "Wizzy/Renderer/Shader.h"
#include "Wizzy/Resource/ResourceManagement.h"

namespace Wizzy {

    mat4 Renderer::s_camTransform;
    bool Renderer::s_isReady(false);
    std::unordered_map<Shader*, std::deque<Submission>> Renderer::s_submissions;
    ulib::Queue<Shader*> Renderer::s_shaderQueue;

    void Renderer::Begin(const mat4& camTransform) {
        WZ_CORE_TRACE("Beginning renderer...");
        s_camTransform = camTransform;
        s_isReady = true;
    }
    void Renderer::End() {
        WZ_CORE_ASSERT(s_isReady, "Begin() was not called on Renderer before End()");
        WZ_CORE_TRACE("Flushing renderer...");

        while (!s_shaderQueue.IsEmpty()) {
            Shader* _shader = s_shaderQueue.Pop();
            auto& _submissions = s_submissions[_shader];

            _shader->Bind();
            _shader->UploadMat4("camTransform", s_camTransform);

            WZ_CORE_TRACE("Rendering submissions for shader");
            RenderSubmissions(_submissions);

            _submissions.clear();
        }

        s_isReady = false;
    }

    void Renderer::Submit(const VertexArrayPtr& va,
                            const Material& material,
                            const mat4& transform,
                            RenderMode mode) {
        WZ_CORE_ASSERT(s_isReady, "Begin() was not called on Renderer before Submit()");

        WZ_CORE_ASSERT(!WZ_IS_RESOURCE_HANDLE_NULL(material.shaderHandle),
                        "Shader cannot be null in material when submitting");
        WZ_CORE_ASSERT(WZ_IS_RESOURCE_HANDLE_NULL(material.diffuseTextureHandle)
             || ResourceManagement::Is<Texture>(material.diffuseTextureHandle),
             "Texture handle must be either null or valid (not invalid) when submitting material")
        WZ_CORE_TRACE("Pushing submission to renderer...");
        auto _shader = ResourceManagement::Get<Shader>(material.shaderHandle);
        WZ_CORE_ASSERT(_shader != nullptr, "Failed retrieving shader with material handle, cannot submit");
        s_shaderQueue.Push(_shader);
        s_submissions[_shader].push_back({ va, material, transform, mode });
    }

    void Renderer::RenderSubmissions(std::deque<Submission>& submissions) {
        for (auto& _submission : submissions) {
            auto& _material = _submission.material;
            auto _shader = ResourceManagement::Get<Shader>(_material.shaderHandle);

            _material.Bind();

            _submission.va->Bind();

            _shader->UploadMat4("worldTransform", _submission.transform);

            RenderCommand::DrawIndexed(_submission.va, _submission.renderMode);
        }
    }
}
