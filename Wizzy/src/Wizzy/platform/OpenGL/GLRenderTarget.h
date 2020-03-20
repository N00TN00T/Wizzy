#pragma once

#include "Wizzy/Renderer/RenderTarget.h"

namespace Wizzy {
    class GLRenderTarget
        : public RenderTarget {
    public:
        GLRenderTarget(u32 width, u32 height);
        GLRenderTarget(const ResData& data, const PropertyLibrary& props);
        ~GLRenderTarget();

        virtual
        void Bind() const override;
        virtual
        void Unbind() const override;

        virtual void BindTexture(u32 location) const override;
        virtual void UnbindTexture() const override;

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

        virtual
        void SetSize(u32 width, u32 height) override;

        virtual ResData Serialize() const override;

    private:
        u32 CreateTexture(u32 width, u32 height, byte* data = NULL);
        u32 CreateRenderBuffer(u32 width, u32 height);
        void SetTexture(u32 textureId, u32 width, u32 height, byte* data = NULL);
        void SetRenderBuffer(u32 renderBufferId, u32 width, u32 height);

        void Init(byte* data = NULL);

    private:
        u32 m_frameBufferId, m_renderBufferId, m_textureId;
        u32 m_width, m_height;

        
    };
}
