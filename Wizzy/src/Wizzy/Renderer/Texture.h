#pragma once

#include "Wizzy/Resource/Resource.h"

namespace Wizzy {

    class RenderTarget;

    class Texture : public Resource 
    {
    public:
        __HANDLE_DEF;
        Texture(const ResData& data, const PropertyTable& flags);
        Texture(byte *rawData, int32 width, int32 height, int32 channels, const PropertyTable& flags = PropertyTable());
        virtual ~Texture();

        
        static const PropertyTable& GetTemplateProps();

        virtual void Bind(int32 location) const = 0;
        virtual void Unbind() const = 0;

        inline int32 GetWidth() const { return m_width; }
        inline int32 GetHeight() const { return m_height; }
        inline int32 GetChannels() const { return m_channels; }

        virtual void AddSubTextureData(byte* data, s32 width, s32 height, s32 posX, s32 posY, s32 channels) = 0;
        // Copies data of given texture into this texture at given position.
        // Does NOT take ownership of Texture*
        virtual void AddSubTexture(Texture* texture, s32 posX, s32 posY) = 0;

        virtual std::shared_ptr<byte> GetData() const = 0;
        virtual void SetData(byte* data, s32 channels, s32 width, s32 height) = 0;

        inline u32 GetId() const { return m_textureId; }

        static
        Resource* Create(const ResData& data, const PropertyTable& props);
        static
        Texture* Create(byte *rawData, int32 width, int32 height, int32 channels, const PropertyTable& props = PropertyTable());
        
        inline static bool IsFileBinary() { return true; }

    protected:
        int32               m_width, m_height, m_channels;
        u32                 m_textureId = WZ_TEXTURE_ID_INVALID;
    private:
        static PropertyTable* s_templateProps;
    };
}
