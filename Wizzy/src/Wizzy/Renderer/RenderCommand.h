#pragma once

#include "Wizzy/Renderer/RendererAPI.h"

namespace Wizzy {
    class RenderCommand {
    public:

        inline static void SetClearColor(float r, float g, float b, float a) 
        {
            s_rendererAPI->SetClearColor(r, g, b, a);
        }
        inline static void SetClearColor(const color3& color) 
        {
            s_rendererAPI->SetClearColor(color);
        }

        inline static const color3& GetClearColor()
        {
            return s_rendererAPI->GetClearColor();
        }

        inline static void DrawIndexed(
            const VertexArrayPtr& va, 
            u32 indexCount, 
            RenderMode mode = WZ_RENDER_MODE_TRIANGLES) 
        {
            s_rendererAPI->DrawIndexed(va, indexCount, mode);
        }

        inline static void SetViewport(const Viewport& vp) 
        {
            s_rendererAPI->SetViewport(vp);
        }

        inline static void SetCullMode(CullMode mode) 
        {
            s_rendererAPI->SetCullMode(mode);
        }

        inline static void ToggleDepthTesting(bool value) 
        {
            s_rendererAPI->ToggleDepthTesting(value);
        }

        inline static void ToggleBlending(bool value) 
        {
            s_rendererAPI->ToggleBlending(value);
        }

        inline static void Clear() { s_rendererAPI->Clear(); }
    private:
        static RendererAPI *s_rendererAPI;
    };
}
