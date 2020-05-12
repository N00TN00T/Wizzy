#include <wzpch.h>

#include "Wizzy/Renderer/Texture.h"
#include "Wizzy/platform/OpenGL/GLTexture.h"
#include "Wizzy/Renderer/Renderer.h"
#include "Wizzy/Resource/ResourceManagement.h"
#include "Wizzy/WizzyExceptions.h"

#define TO_4_BYTE_COLOR(c16) new byte[4] { (byte)(c16.r * 255.f), (byte)(c16.g * 255.f), (byte)(c16.b * 255.f), (byte)(c16.a * 255.f) }

namespace Wizzy
{

    PropertyTable* Texture::s_templateProps;

    Texture::Texture(const ResData& data, const PropertyTable& props)
        : Resource(props)
    {
        /*auto file = props.GetProperty<string>("SourceFile");
        stbi_set_flip_vertically_on_load(true);

        m_data = stbi_load(file.c_str(), &m_width, &m_height, &m_channels, 4);

        if (m_data) {
            WZ_CORE_INFO("Succesfully loaded Texture (W: '{0}', H: '{1}', C: {2})", m_width, m_height, m_channels);
        } else {
            WZ_THROW(Exception, "Failed loading texture: '" + string(stbi_failure_reason()) + "'");
            return;
        }*/
    }

    Texture::Texture(byte* rawData, int32 width, int32 height, int32 channels, const PropertyTable& flags)
        : Resource(flags),
        m_width(width), m_height(height), m_channels(channels)
    {
        WZ_CORE_ASSERT(width > 0, "Width must be > 0 on textures");
        WZ_CORE_ASSERT(height > 0, "Height must be > 0 on textures");
        WZ_CORE_ASSERT(channels > 0 && channels <= 4, "Channel count must be between 1 and 4");
    }
    Texture::~Texture()
    {

    }

    const PropertyTable& Texture::GetTemplateProps()
    {
        if (!s_templateProps)
        {
            s_templateProps = new PropertyTable;
            s_templateProps->Set<int32>("WrapMode", (int32)WZ_WRAP_MODE_CLAMP_TO_EDGE);
            s_templateProps->Set<int32>("MinFilterMode", (int32)WZ_MIN_FILTER_MODE_LINEAR_MIPMAP_LINEAR);
            s_templateProps->Set<int32>("MagFilterMode", (int32)WZ_MAG_FILTER_LINEAR);
        }
        return *s_templateProps;
    }

    Resource* Texture::Create(const ResData& data, const PropertyTable& props)
    {
        CREATE_BY_API(new GLTexture(data, props));

        return nullptr;
    }

    Texture* Texture::Create(byte* rawData, int32 width, int32 height, int32 channels, const PropertyTable& props)
    {
        CREATE_BY_API(new GLTexture(rawData, width, height, channels, props));

        return nullptr;
    }

}
