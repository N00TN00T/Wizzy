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
        void DrawIndexed(const VertexArrayPtr& va) {
            s_rendererAPI->DrawIndexed(va);
        }

        inline static
        void SetViewport(u32 x, u32 y, u32 w, u32 h) {
            s_rendererAPI->SetViewport(x, y, w, h);
        }
        inline static
        void SetViewport(vec2 pos, vec2 size) {
            s_rendererAPI->SetViewport(pos, size);
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
        void Clear() { s_rendererAPI->Clear(); }
    private:
        static RendererAPI *s_rendererAPI;
    };
}