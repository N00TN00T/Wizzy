#pragma once

#include "Wizzy/Renderer/RenderTarget.h"

namespace Wizzy {
    class GLRenderTarget
        : public RenderTarget {
    public:
        GLRenderTarget(s32 width, s32 height, s32 channels);
        GLRenderTarget(const ResData& data, const PropertyTable& props);
        ~GLRenderTarget();

        void Bind() const override;
        void Unbind() const override;

        void BindTexture(u32 location) const override;
        void UnbindTexture() const override;

        inline u32 GetTextureId() const override { return m_textureId; }
        inline s32 GetWidth() const override { return m_width; }
        inline s32 GetHeight() const override { return m_height; }

        void SetSize(s32 width, s32 height) override;

        ResData Serialize() const override;

    private:
        u32 CreateTexture(s32 width, s32 height, s32 channels, byte* data = NULL);
        void SetTexture(u32 textureId, s32 width, s32 height, s32 channels, byte* data = NULL);
        void Init(byte* data = NULL);

    private:
        u32 m_frameBufferId, m_textureId;
        s32 m_width, m_height, m_channels;

        
    };
}
