#pragma once

namespace Wizzy {
    class GLTexture
        : public Texture {
    public:
        GLTexture(const string& file, const ulib::Bitset& flags);
        ~GLTexture();

        virtual
        bool IsGarbage() const override {
            return m_textureId == WZ_TEXTURE_ID_INVALID;
        }

        virtual
        void Bind(int32 location) const override;
        virtual
        void Unbind() const override;

        virtual
        void Delete() override;

        inline virtual

        u32 GetId() const override { return m_textureId; }
    private:
        virtual
        void Init() override;

    private:
        ulib::Bitset        m_flags;
        u32                 m_textureId = WZ_TEXTURE_ID_INVALID;
    };
}
