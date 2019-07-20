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
        virtual
        void SetClearColor(float r, float g, float b, float a) = 0;
        inline
        void SetClearColor(const vec4& color) {
            SetClearColor(color.x, color.y, color.z, color.w);
        }
        virtual
        void Clear() = 0;

        virtual
        void DrawIndexed(const VertexArrayPtr& va, RenderMode mode) = 0;

        virtual
        void SetViewport(const Viewport& vp) = 0;

        virtual
        void SetCullMode(CullMode mode) = 0;
        virtual
        void ToggleDepthTesting(bool value) = 0;
        virtual
        void ToggleBlending(bool value) = 0;

        inline static
        int8 GetAPI() { return s_API; }
        inline static
        void SetAPI(int8 API) { s_API = API; }

    private:
        static int8 s_API;
    };
}
