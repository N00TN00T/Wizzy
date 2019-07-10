#pragma once

#include "Wizzy/Resource/Resource.h"

namespace Wizzy {

    class Texture
        : public Resource {
        public:
            Texture(const string& data, const Flagset& flags);
            Texture(byte *rawData, int32 width, int32 height,
                    const Flagset& flags = Flagset());
            virtual ~Texture();

            virtual
            string Serialize() const override;

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
            Texture* Create(const string& sourceFile,
                            const string& data,
                            const Flagset& flags);
            static
            Texture* Create(byte *rawData, int32 width, int32 height,
                            const Flagset& flags = Flagset());

            static
            TextureHandle UnloadedTexture();
            static
            TextureHandle InvalidTexture();


        protected:
            byte*               m_data;
            int32               m_width, m_height, m_channels;
    };
}
