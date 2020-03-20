#pragma once

#include "Wizzy/Resource/Resource.h"

namespace Wizzy {

    class RenderTarget;

    class Texture : public Resource 
    {
    public:
        __HANDLE_DEF;
        Texture(const ResData& data, const PropertyLibrary& flags);
        Texture(byte *rawData, int32 width, int32 height, int32 channels, const PropertyLibrary& flags = *s_templateProps);
        virtual ~Texture();

        
        static const PropertyLibrary& GetTemplateProps();

        virtual
        void Bind(int32 location) const = 0;
        virtual
        void Unbind() const = 0;

        inline
        int32 GetWidth() const { return m_width; }
        inline
        int32 GetHeight() const { return m_height; }
        inline
        int32 GetChannels() const { return m_channels; }

        virtual void AddSubTexture(byte* data, int32 width, int32 height, int32 posX, int32 posY, int32 channels) = 0;

        inline u32 GetId() const { return m_textureId; }

        static
        Resource* Create(const ResData& data, const PropertyLibrary& props);
        static
        Texture* Create(byte *rawData, int32 width, int32 height, int32 channels, const PropertyLibrary& props = *s_templateProps);

    protected:
        int32               m_width, m_height, m_channels;
        u32                 m_textureId = WZ_TEXTURE_ID_INVALID;
    private:
        static PropertyLibrary* s_templateProps;
    };
}
