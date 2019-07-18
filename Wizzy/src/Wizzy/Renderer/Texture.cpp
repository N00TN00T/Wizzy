#include <wzpch.h>

#include <stb_image.h>

#include "Wizzy/Renderer/Texture.h"
#include "Wizzy/platform/OpenGL/GLTexture.h"
#include "Wizzy/Utils.h"
#include "Wizzy/Renderer/Renderer.h"
#include "Wizzy/Resource/ResourceManagement.h"

#define TO_4_BYTE_COLOR(c16) new byte[4] { (byte)(c16.r * 255.f), (byte)(c16.g * 255.f), (byte)(c16.b * 255.f), (byte)(c16.a * 255.f) }

namespace Wizzy {

    Texture::Texture(const string& data, const Flagset& flags)
        : Resource(flags, "Texture", WZ_EXTENSION_TEXTURE) {
        WZ_CORE_TRACE("Loading Texture from resource data...");

        stbi_set_flip_vertically_on_load(true);

        m_data = stbi_load_from_memory((byte*)((void*)&data[0]), data.size(), &m_width, &m_height,
                                        &m_channels, 4);

        if (m_data) {
            WZ_CORE_INFO("Succesfully loaded Texture (W: '{0}', H: '{1}', C: {2})", m_width, m_height, m_channels);
        } else {
            WZ_CORE_ERROR("Failed loading texture, invalid data.");
            m_isValid = false;
        }
    }

    Texture::Texture(byte *rawData, int32 width, int32 height, const Flagset& flags)
        : Resource(flags, "Texture", WZ_EXTENSION_TEXTURE), m_data(rawData),
          m_width(width), m_height(height), m_channels(4) {
        WZ_CORE_ASSERT(width > 0, "Width must be > 0 on textures");
        WZ_CORE_ASSERT(height > 0, "Height must be > 0 on textures");
        WZ_CORE_ASSERT(rawData != nullptr, "Data cannot be null in textures");
    }
    Texture::~Texture() {

    }

    string Texture::Serialize() const {
        WZ_CORE_ERROR("Texture serialization not yet implemented");
        return "";
    }

    Texture* Texture::Create(const string& sourceFile, const string& data, const Flagset& flags) {

        CREATE_BY_API(new GLTexture(data, flags));

        return nullptr;
    }

    Texture* Texture::Create(byte *rawData, int32 width, int32 height, const Flagset& flags) {

        CREATE_BY_API(new GLTexture(rawData, width, height, flags));

        return nullptr;
    }

    TextureHandle Texture::UnloadedTexture() {
        if (!ResourceManagement::IsValid("UnloadedTexture")) {
            byte *_color = TO_4_BYTE_COLOR(WZ_UNLOADED_TEXTURE_COLOR);
            Texture *_texture = Texture::Create(
                                                _color,
                                                1, 1
                                            );
            ResourceManagement::Add(_texture, "UnloadedTexture");
        }
        return "UnloadedTexture";
    }
    TextureHandle Texture::InvalidTexture() {
        if (!ResourceManagement::IsValid("InvalidTexture")) {
            byte *_color = TO_4_BYTE_COLOR(WZ_INVALID_TEXTURE_COLOR);

            Texture *_texture = Texture::Create(
                                                _color,
                                                1, 1
                                            );
            ResourceManagement::Add(_texture, "InvalidTexture");
        }
        return "InvalidTexture";
    }

    TextureHandle Texture::WhiteTexture() {
        if (!ResourceManagement::IsValid("WhiteTexture")) {
            byte *_color = TO_4_BYTE_COLOR(Color::white);
            Texture *_texture = Texture::Create(
                                                _color,
                                                1, 1
                                            );
            ResourceManagement::Add(_texture, "WhiteTexture");
        }
        return "WhiteTexture";
    }

}
