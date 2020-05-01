#pragma once

#include "Wizzy/Renderer/RendererAPI.h"

namespace Wizzy {
    class GLRendererAPI
        : public RendererAPI {
    public:
        virtual
        void SetClearColor(float r, float g, float b, float a) override;

        const Color& GetClearColor() const override;

        virtual
        void Clear() override;

        virtual
        void SetViewport(const Viewport& vp) override;

        virtual
        void SetCullMode(CullMode mode) override;
        virtual
        void ToggleDepthTesting(bool value) override;
        virtual
        void ToggleBlending(bool value) override;

        virtual
        void DrawIndexed(const VertexArrayPtr& va, u32 indexCount, RenderMode mode) override;
    private:
        bool m_cullingEnabled = false;
        bool m_depthTestingEnabled = false;
        bool m_blendingEnabled = false;
    };
}
