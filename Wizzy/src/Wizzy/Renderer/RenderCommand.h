#pragma once

#include "Wizzy/Renderer/RendererAPI.h"

namespace Wizzy {
    class RenderCommand {
    public:

        inline static
        void SetClearColor(float r, float g, float b, float a) {
            s_rendererAPI->SetClearColor(r, g, b, a);
        }
        inline static
        void SetClearColor(vec4 color) {
            s_rendererAPI->SetClearColor(color);
        }

        inline static
        void DrawIndexed(const VertexArrayPtr& va, RenderMode mode) {
            s_rendererAPI->DrawIndexed(va, mode);
        }

        inline static
        void SetViewport(const Viewport& vp) {
            s_rendererAPI->SetViewport(vp);
        }

        inline static
        void SetCullMode(CullMode mode) {
            s_rendererAPI->SetCullMode(mode);
        }

        inline static
        void ToggleDepthTesting(bool value) {
            s_rendererAPI->ToggleDepthTesting(value);
        }

        inline static
        void ToggleBlending(bool value) {
            s_rendererAPI->ToggleBlending(value);
        }

        inline static
        void Clear() { s_rendererAPI->Clear(); }
    private:
        static RendererAPI *s_rendererAPI;
    };
}
