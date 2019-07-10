#include "wzpch.h"

#include "Wizzy/Renderer/Renderer.h"
#include "Wizzy/Renderer/Shader.h"
#include "Wizzy/Resource/ResourceManagement.h"
#include "Wizzy/Stopwatch.h"

namespace Wizzy {

    mat4 Renderer::s_camTransform;
    bool Renderer::s_isReady(false);
    std::unordered_map<ShaderHandle, std::deque<Submission>> Renderer::s_submissions;
    ulib::Queue<ShaderHandle> Renderer::s_shaderQueue;

    void Renderer::Begin(const mat4& camTransform) {
        WZ_CORE_TRACE("Beginning renderer...");
        s_camTransform = camTransform;
        s_isReady = true;
    }
    void Renderer::End() {
        WZ_CORE_ASSERT(s_isReady, "Begin() was not called on Renderer before End()");
        WZ_CORE_TRACE("Flushing renderer...");

        while (!s_shaderQueue.IsEmpty()) {

            auto _shaderHandle = s_shaderQueue.Pop();
            Shader *_shader = ResourceManagement::Get<Shader>(_shaderHandle);
            auto& _submissions = s_submissions[_shaderHandle];

            _shader->Bind();
            _shader->UploadMat4("camTransform", s_camTransform);

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

        WZ_CORE_ASSERT(ResourceManagement::Is<Shader>(material.shaderHandle), "Shader was either null handle or wrong type in renderer submit");
        WZ_CORE_TRACE("Pushing submission  to renderer...");
        s_shaderQueue.Push(material.shaderHandle);
        s_submissions[material.shaderHandle].push_back({ va, material, transform, mode });
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
