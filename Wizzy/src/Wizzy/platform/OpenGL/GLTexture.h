#pragma once

namespace Wizzy {
    class GLTexture
        : public Texture {
    public:
        GLTexture(const ResData& data, const PropertyTable& flags);
        GLTexture(byte *rawData, int32 width, int32 height,
                int32 channels, const PropertyTable& props = PropertyTable());
        ~GLTexture();

        void Bind(int32 location) const override;
        void Unbind() const override;

        void AddSubTextureData(byte* data, int32 width, int32 height, int32 posX, int32 posY, int32 channels) override;

        void AddSubTexture(Texture* texture, s32 posX, s32 posY) override;

        std::shared_ptr<byte> GetData() const override;
        void SetData(byte* data, s32 channels, s32 width, s32 height) override;

        ResData Serialize() const override;

    private:
        void Init(byte* data); 
    };
}
