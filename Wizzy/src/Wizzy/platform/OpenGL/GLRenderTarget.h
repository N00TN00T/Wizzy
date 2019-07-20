#pragma once

namespace Wizzy {
    class GLRenderTarget
        : public RenderTarget {
    public:
        GLRenderTarget(u32 width, u32 height);
        ~GLRenderTarget();

        virtual
        void Bind() const override;
        virtual
        void Unbind() const override;

        inline virtual
        u32 GetTextureId() const override {
            return m_textureId;
        }
        inline virtual
        u32 GetWidth() const override {
            return m_width;
        }
        inline virtual
        u32 GetHeight() const override {
            return m_height;
        }

    private:
        u32 CreateTexture(u32 width, u32 height);
        u32 CreateRenderBuffer(u32 width, u32 height);

    private:
        u32 m_frameBufferId, m_renderBufferId, m_textureId;
        u32 m_width, m_height;
    };
}
