#include <wzpch.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "Wizzy/Renderer/Texture.h"
#include "Wizzy/platform/OpenGL/GLTexture.h"
#include "Wizzy/Utils.h"
#include "Wizzy/Renderer/Renderer.h"
#include "Wizzy/Resource/ResourceManagement.h"
#include "Wizzy/WizzyExceptions.h"

#define TO_4_BYTE_COLOR(c16) new byte[4] { (byte)(c16.r * 255.f), (byte)(c16.g * 255.f), (byte)(c16.b * 255.f), (byte)(c16.a * 255.f) }

namespace Wizzy {

    PropertyLibrary* Texture::s_templateProps;

    Texture::Texture(const ResData& data, const PropertyLibrary& props)
        : Resource(props) 
    {
        auto file = props.GetProperty<string>("SourceFile");
        stbi_set_flip_vertically_on_load(true);

        m_data = stbi_load(file.c_str(), &m_width, &m_height, &m_channels, 4);

        if (m_data) {
            WZ_CORE_INFO("Succesfully loaded Texture (W: '{0}', H: '{1}', C: {2})", m_width, m_height, m_channels);
        } else {
            WZ_THROW(Exception, "Failed loading texture: '" + string(stbi_failure_reason()) + "'");
			return;
        }
    }

    Texture::Texture(byte* rawData, int32 width, int32 height, const PropertyLibrary& flags)
        : Resource(flags), m_data(rawData),
          m_width(width), m_height(height), m_channels(4) {
        WZ_CORE_ASSERT(width > 0, "Width must be > 0 on textures");
        WZ_CORE_ASSERT(height > 0, "Height must be > 0 on textures");
        WZ_CORE_ASSERT(rawData != nullptr, "Data cannot be null in textures");
    }
    Texture::~Texture() {
		if (m_data) stbi_image_free(m_data);
    }

    ResData Texture::Serialize() const {
        int32 len;
        stbi_flip_vertically_on_write(true);
        byte* serializedBytes = stbi_write_png_to_mem(m_data, 0, m_width, m_height, m_channels, &len);
        ResData data(serializedBytes, serializedBytes + len);
        delete serializedBytes;
        return data;
    }
    
    const PropertyLibrary& Texture::GetTemplateProps()
    {
        if (!s_templateProps)
        {
            s_templateProps = new PropertyLibrary;
            s_templateProps->SetProperty<int32>("WrapMode", (int32)WZ_WRAP_MODE_REPEAT);
            s_templateProps->SetProperty<int32>("MinFilterMode", (int32)WZ_MIN_FILTER_MODE_LINEAR_MIPMAP_LINEAR);
            s_templateProps->SetProperty<int32>("MagFilterMode", (int32)WZ_MAG_FILTER_LINEAR);
        }
        return *s_templateProps;
    }

    Resource* Texture::Create(const ResData& data, const PropertyLibrary& props) {

        CREATE_BY_API(new GLTexture(data, props));

        return nullptr;
    }

    Texture* Texture::Create(byte* rawData, int32 width, int32 height, const PropertyLibrary& props) {

        CREATE_BY_API(new GLTexture(rawData, width, height, props));

        return nullptr;
    }

}
