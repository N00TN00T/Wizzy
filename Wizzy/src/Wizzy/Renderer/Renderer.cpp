#include "wzpch.h"

#include "Wizzy/Renderer/Renderer.h"
#include "Wizzy/Renderer/Shader.h"
#include "Wizzy/Resource/ResourceManagement.h"
#include "Wizzy/Stopwatch.h"

namespace Wizzy {

    mat4 Renderer::s_camTransform;
    vec3 Renderer::s_viewPos;
    RenderEnvironment Renderer::s_environment;
    std::vector<Light> Renderer::s_lights;
    bool Renderer::s_isReady(false);
    std::unordered_map<ShaderHandle, std::deque<Submission>> Renderer::s_submissions;
    ulib::Queue<ShaderHandle> Renderer::s_shaderQueue;

    void Renderer::Begin(const mat4& camTransform, const vec3& viewPos, const RenderEnvironment& environment) {
        WZ_CORE_TRACE("Beginning renderer...");
        s_camTransform = camTransform;
        s_viewPos = viewPos;
        WZ_CORE_DEBUG("{0}, {1}, {2}", s_viewPos.x, s_viewPos.y, s_viewPos.z);
        s_environment = environment;
        s_isReady = true;
    }
    void Renderer::End() {
        WZ_CORE_ASSERT(s_isReady, "Begin() was not called on Renderer before End()");
        WZ_CORE_TRACE("Flushing renderer...");

        while (!s_shaderQueue.IsEmpty()) {

            auto _shaderHandle = s_shaderQueue.Pop();
            Shader& _shader = ResourceManagement::Get<Shader>(_shaderHandle);
            auto& _submissions = s_submissions[_shaderHandle];

            _shader.Bind();
            _shader.UploadMat4("u_camTransform", s_camTransform);
            _shader.Upload3f("u_viewPos", s_viewPos);

            if (s_environment.useLighting) {
                _shader.Upload4f("u_ambient", s_environment.ambient.asVec4);

                _shader.Upload1i("u_nLights", s_lights.size());

                for (u32 i = 0; i < s_lights.size(); i++) {
                    _shader.Upload1i("u_lights[" + std::to_string(i) + "].type", (int32)s_lights[i].type);
                    _shader.Upload3f("u_lights[" + std::to_string(i) + "].position", s_lights[i].position);
                    _shader.Upload3f("u_lights[" + std::to_string(i) + "].direction", s_lights[i].rotation);
                    _shader.Upload4f("u_lights[" + std::to_string(i) + "].color", s_lights[i].color.asVec4);
                    _shader.Upload1f("u_lights[" + std::to_string(i) + "].range", s_lights[i].range);
                    _shader.Upload1f("u_lights[" + std::to_string(i) + "].intensity", s_lights[i].intensity);
                }
            }

            WZ_CORE_TRACE("Renderings submissions submitted with shader '{0}'", _shaderHandle);
            RenderSubmissions(_submissions);

            _submissions.clear();
        }

        s_lights.clear();

        s_isReady = false;
    }

    void Renderer::SubmitLight(LightType type,
                               const vec3& position,
                               const vec3& rotation,
                               const Color& color,
                               float range,
                               float intensity) {
        WZ_CORE_TRACE("Submitting light...");
        s_lights.push_back({type, position, rotation, color, range, intensity});
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
            auto& _shader = ResourceManagement::Get<Shader>(_material.shaderHandle);
            _material.Bind();
            _submission.va->Bind();
            _submission.va->GetIndexBuffer()->Bind();
            _shader.UploadMat4("u_worldTransform", _submission.transform);
            RenderCommand::DrawIndexed(_submission.va, _submission.renderMode);

            _submission.va->Unbind();
            _submission.va->GetIndexBuffer()->Unbind();
        }
    }
}
