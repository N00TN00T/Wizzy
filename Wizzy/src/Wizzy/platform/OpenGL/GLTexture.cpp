#include "wzpch.h"

#include <glad/glad.h>

#define STB_IMAGE_STATIC


#include "Wizzy/Renderer/Texture.h"
#include "Wizzy/platform/OpenGL/GLTexture.h"
#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include "Wizzy/platform/OpenGL/GLAPI.h"
#include "Wizzy/WizzyExceptions.h"
#include "Wizzy/ImageImporter.h"

#define DEFAULT_WRAP_MODE       (GL_REPEAT)
#define DEFAULT_MIN_FILTER_MODE (GL_LINEAR_MIPMAP_LINEAR)
#define DEFAULT_MAG_FILTER_MODE (GL_LINEAR)

namespace Wizzy {
    GLTexture::GLTexture(const ResData& encoded, const PropertyTable& flags)
        : Texture(encoded, flags) {
        Image::set_vertical_flip_on_load(true);
        auto decoded = Image::decode_image(encoded.data(), encoded.size(), m_width, m_height, m_channels);
        if (decoded)
        {
            Init(decoded);
            Image::free_image(decoded);
        }
        else
        {
            WZ_THROW(Exception, "Failed loading texture: '" + string(Image::get_failure_reason()) + "'");
            return;
        }
    }

    GLTexture::GLTexture(byte *rawData, int32 width, int32 height, int32 channels, const PropertyTable& props)
        : Texture(rawData, width, height, channels, props) {
        Init(rawData);
    }

    GLTexture::~GLTexture() {
        WZ_CORE_TRACE("Deleting GL Texture with id '{0}'", m_textureId);
        GL_CALL(glDeleteTextures(1, &m_textureId));
    }

    void GLTexture::Bind(int32 location) const {
        GL_CALL(glActiveTexture(GL_TEXTURE0 + location));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, m_textureId));
    }

    void GLTexture::Unbind() const {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }

	void GLTexture::AddSubTexture(byte* data, int32 width, int32 height, int32 posX, int32 posY, int32 channels)
	{
        GLenum fmt = GL_RGBA;
        switch (channels)
        {
        case 1: fmt = GL_RED; break;
        case 2: fmt = GL_RG; break;
        case 3: fmt = GL_RGB; break;
        case 4: fmt = GL_RGBA; break;
        }
        this->Bind(1);
        GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0, posX, posY, width, height, fmt, GL_UNSIGNED_BYTE, data));
        this->Unbind();
	}

    ResData GLTexture::Serialize() const
    {
        byte* pixels = (byte*)malloc((int64)m_width * (int64)m_height * (int64)m_channels);

        glBindTexture(GL_TEXTURE_2D, m_textureId);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_TextureChannelsToAPIFormat(m_channels), GL_UNSIGNED_BYTE, pixels);
        glBindTexture(GL_TEXTURE_2D, 0);

        int32 len = 0;
        Image::set_vertical_flip_on_write(true);
        byte* serializedBytes = Image::encode_png(pixels, m_width, m_height, m_channels, len);
        ResData data(serializedBytes, serializedBytes + len);
        
        delete serializedBytes;
        free(pixels);

        return data;
    }

	void GLTexture::Init(byte* data) {
		WZ_CORE_TRACE("Initializing GL Texture...");

        GL_CALL(glGenTextures(1, &m_textureId));

        GL_CALL(glBindTexture(GL_TEXTURE_2D, m_textureId));

        WZ_CORE_TRACE("Generated GL texture with id '{0}'", m_textureId);

        GLenum _wrapMode = DEFAULT_WRAP_MODE;
        GLenum _minFilterMode = DEFAULT_MIN_FILTER_MODE;
        GLenum _magFilterMode = DEFAULT_MAG_FILTER_MODE;

        int32 _wrapModeBit = m_props.Is<int32>("WrapMode") ?
            m_props.Get<int32>("WrapMode")
            : GetTemplateProps().Get<int32>("WrapMode");
        m_props.Set<int32>("WrapMode", _wrapModeBit);

        switch (_wrapModeBit)
        {
        case WZ_WRAP_MODE_CLAMP_TO_EDGE: _wrapMode = GL_CLAMP_TO_EDGE; break;
        case WZ_WRAP_MODE_MIRRORED_REPEAT: _wrapMode = GL_MIRRORED_REPEAT; break;
        case WZ_WRAP_MODE_REPEAT: _wrapMode = GL_REPEAT; break;
        }

        int32 _minFilterModeBit = m_props.Is<int32>("MinFilterMode") ?
            m_props.Get<int32>("MinFilterMode")
            : GetTemplateProps().Get<int32>("MinFilterMode");
        m_props.Set<int32>("MinFilterMode", _minFilterModeBit);

        switch (_minFilterModeBit)
        {
        case WZ_MIN_FILTER_MODE_NEAREST: _minFilterMode = GL_NEAREST; break;
        case WZ_MIN_FILTER_MODE_LINEAR: _minFilterMode = GL_LINEAR; break;
        case WZ_MIN_FILTER_MODE_NEAREST_MIPMAP_NEAREST: _minFilterMode = GL_NEAREST_MIPMAP_NEAREST; break;
        case WZ_MIN_FILTER_MODE_LINEAR_MIPMAP_NEAREST: _minFilterMode = GL_LINEAR_MIPMAP_NEAREST; break;
        case WZ_MIN_FILTER_MODE_NEAREST_MIPMAP_LINEAR: _minFilterMode = GL_NEAREST_MIPMAP_LINEAR; break;
        case WZ_MIN_FILTER_MODE_LINEAR_MIPMAP_LINEAR: _minFilterMode = GL_LINEAR_MIPMAP_LINEAR; break;
        }

        int32 _magFilterModeBit = m_props.Is<int32>("MagFilterMode") ?
            m_props.Get<int32>("MagFilterMode")
            : GetTemplateProps().Get<int32>("MagFilterMode");
        m_props.Set<int32>("MagFilterMode", _magFilterModeBit);

        switch (_magFilterModeBit)
        {
        case WZ_MAG_FILTER_NEAREST: _magFilterMode = GL_NEAREST; break;
        case WZ_MAG_FILTER_LINEAR: _magFilterMode = GL_LINEAR; break;
        }

        WZ_CORE_TRACE("Setting texture parameters...");
        GL_CALL(glTexParameteri(GL_TEXTURE_2D,
                                GL_TEXTURE_WRAP_S,
                                _wrapMode));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D,
                                GL_TEXTURE_WRAP_T,
                                _wrapMode));

        GL_CALL(glTexParameteri(GL_TEXTURE_2D,
                                GL_TEXTURE_MIN_FILTER,
                                _minFilterMode));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D,
                                GL_TEXTURE_MAG_FILTER,
                                _magFilterMode));

        GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

        GLenum fmt = GL_TextureChannelsToAPIFormat(m_channels);

        WZ_CORE_TRACE("Creating GL tex2d depending on given image type in flags");
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, fmt, m_width, m_height,
                             0, fmt, GL_UNSIGNED_BYTE, data));

        WZ_CORE_TRACE("Generating mitmaps");
        
        bool mipmap = _minFilterModeBit == WZ_MIN_FILTER_MODE_LINEAR_MIPMAP_LINEAR || _minFilterModeBit == WZ_MIN_FILTER_MODE_LINEAR_MIPMAP_NEAREST || _minFilterModeBit == WZ_MIN_FILTER_MODE_NEAREST_MIPMAP_LINEAR || _minFilterModeBit == WZ_MIN_FILTER_MODE_NEAREST_MIPMAP_NEAREST;

        if (mipmap) {
            GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
        }

       

        WZ_CORE_INFO("Successfully Initialized GL Texture (w: {0}, h: {1}, c:{2}) and assigned id '{3}'", m_width, m_height, m_channels, m_textureId);
    }
}
