#pragma once

namespace Wizzy {
    class GLTexture
        : public Texture {
    public:
        GLTexture(const string& data, const ulib::Bitset& flags);
        GLTexture(byte *rawData, int32 width, int32 height,
                const Flagset& flags = Flagset());
        ~GLTexture();

        virtual
        void Bind(int32 location) const override;
        virtual
        void Unbind() const override;

        inline virtual
        u32 GetId() const override { return m_textureId; }

    private:
        void Init();

    private:
        u32                 m_textureId = WZ_TEXTURE_ID_INVALID;
    };
}
