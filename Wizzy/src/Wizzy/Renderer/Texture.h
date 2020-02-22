#pragma once

#include "Wizzy/Resource/Resource.h"

namespace Wizzy {

    class Texture : public Resource 
    {
    public:
        __HANDLE_DEF;
    public:
        Texture(const ResData& data, const PropertyLibrary& flags);
        Texture(byte *rawData, int32 width, int32 height, const PropertyLibrary& flags = *s_templateProps);
        virtual ~Texture();

        virtual
            ResData Serialize() const override;
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

        virtual
        u32 GetId() const = 0;

        static
        Resource* Create(const ResData& data, const PropertyLibrary& props);
        static
        Texture* Create(byte *rawData, int32 width, int32 height, const PropertyLibrary& props = *s_templateProps);

    protected:
        byte*               m_data;
        int32               m_width, m_height, m_channels;

    private:
        static PropertyLibrary* s_templateProps;

    };
}
