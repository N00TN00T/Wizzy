#pragma once

#include "Wizzy/Resource/IResource.h"

namespace Wizzy {

    typedef ResourceHandle TextureHandle;

    class Texture
        : public IResource {
        public:
            Texture(const string& file)
                : IResource(file, "Texture") {}
            virtual ~Texture();

            virtual
            void        Load() override;
            virtual
            void        Unload() override;
            virtual
            void        Reload() override;
            virtual
            void        Save() override;

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

            virtual
            void Delete() = 0;

            static
            Texture* Create(const string& file,
                            const ulib::Bitset& flags = ulib::Bitset());

        private:
            virtual
            void Init() = 0;

        protected:
            byte*               m_data;
            int32               m_width, m_height, m_channels;
    };
}
