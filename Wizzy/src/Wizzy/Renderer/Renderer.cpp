#include "wzpch.h"

#include "Wizzy/Renderer/Renderer.h"

namespace Wizzy {

    mat4 Renderer::s_camTransform;
    mat4 Renderer::s_projection;
    bool Renderer::s_isReady(false);

    void Renderer::Begin(const mat4& camTransform,
                            const mat4& projection) {
        s_camTransform = camTransform;
        s_projection = projection;
        s_isReady = true;
    }
    void Renderer::End() {
        s_isReady = false;
    }

    void Renderer::Submit(const VertexArrayPtr& va,
                            const Material& material,
                            const mat4& transform) {
        WZ_CORE_ASSERT(s_isReady, "Begin() was not called on Renderer before Submit()");

        va->Bind();

        material.shader->Bind();
        material.shader->SetUniformMat4("projection", s_projection);
        material.shader->SetUniformMat4("view", s_camTransform);
        material.shader->SetUniformMat4("model", transform);

        RenderCommand::DrawIndexed(va);
    }
}
