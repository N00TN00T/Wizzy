#pragma once

#include "Wizzy/Renderer/RendererAPI.h"

namespace Wizzy {
    class GLRendererAPI
        : public RendererAPI {
    public:
        virtual
        void SetClearColor(float r, float g, float b, float a) override;
        virtual
        void Clear() override;

        virtual
        void SetViewport(u32 x, u32 y, u32 w, u32 h) override;

        virtual
        void DrawIndexed(const VertexArrayPtr& va) override;
    private:
    };
}
