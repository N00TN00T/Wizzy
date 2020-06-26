#pragma once

#include "Wizzy/Renderer/Buffers.h"
#include "Wizzy/Renderer/API.h"

namespace Wizzy {
    class RendererAPI {
    public:
        enum {
            API_NONE = 0, API_OPENGL = 1
        };

    public:
        virtual void SetClearColor(float r, float g, float b, float a) = 0;
        inline void SetClearColor(const color3& color)
        {
            SetClearColor(color.r, color.g, color.b, 1.f);
        }

        virtual const color3& GetClearColor() const = 0;

        virtual void Clear() = 0;

        virtual void DrawIndexed(
            const VertexArrayPtr& va, 
            u32 indexCoint, 
            RenderMode mode
        ) = 0;

        virtual void SetViewport(const Viewport& vp) = 0;

        virtual void SetCullMode(CullMode mode) = 0;
        virtual void ToggleDepthTesting(bool value) = 0;
        virtual void ToggleBlending(bool value) = 0;

        inline static int8 GetAPI() { return s_API; }
        inline static void SetAPI(int8 API) { s_API = API; }

    private:
        static int8 s_API;
    };
}
