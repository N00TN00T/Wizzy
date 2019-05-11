#pragma once

#include "Wizzy/Resources/IResource.h"

namespace Wizzy {
    class Texture2D
        : public IResource {
        public:
            Texture2D(const string& file)
                : IResource(file) {}
            ~Texture2D();

            virtual void        Load() override;
            virtual void        Unload() override;
            virtual void        Reload() override;
            virtual void        Save() override;
            inline virtual bool IsGarbage() const override {
                return m_textureId == WZ_TEXTURE_ID_INVALID;
            }

            void                Bind(const int32& location) const;
            void                Unbind() const;

            inline const int32& GetWidth() const {		return m_width;		}
            inline const int32& GetHeight() const {		return m_height;	}
            inline const int32& GetChannels() const {	return m_channels;	}
			inline const u32&	GetVao() const {		return m_vao;		}

        private:
            void                Init();

        private:
            int32               m_width, m_height, m_channels;
            byte*               m_data;
            u32                 m_textureId = WZ_TEXTURE_ID_INVALID;
			u32					m_vao = 0;
    };
}
