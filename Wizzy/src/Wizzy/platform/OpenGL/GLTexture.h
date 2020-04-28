#pragma once

namespace Wizzy {
    class GLTexture
        : public Texture {
    public:
        GLTexture(const ResData& data, const PropertyTable& flags);
        GLTexture(byte *rawData, int32 width, int32 height,
                int32 channels, const PropertyTable& props = PropertyTable());
        ~GLTexture();

        virtual
        void Bind(int32 location) const override;
        virtual
        void Unbind() const override;

        virtual void AddSubTexture(byte* data, int32 width, int32 height, int32 posX, int32 posY, int32 channels) override;

        virtual ResData Serialize() const override;

    private:
        void Init(byte* data); 
    };
}
