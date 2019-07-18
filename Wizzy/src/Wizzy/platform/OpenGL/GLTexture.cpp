#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/Renderer/Texture.h"
#include "Wizzy/platform/OpenGL/GLTexture.h"
#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include "Wizzy/platform/OpenGL/GLAPI.h"

#define DEFAULT_WRAP_MODE       WZ_WRAP_MODE_MIRRORED_REPEAT
//(WZ_WRAP_MODE_REPEAT)
#define DEFAULT_MIN_FILTER_MODE (GL_LINEAR_MIPMAP_LINEAR)
#define DEFAULT_MAG_FILTER_MODE (GL_LINEAR)

namespace Wizzy {
    GLTexture::GLTexture(const string& data, const ulib::Bitset& flags)
        : Texture(data, flags) {
        Init();
    }

    GLTexture::GLTexture(byte *rawData, int32 width, int32 height, const Flagset& flags)
        : Texture(rawData, width, height, flags) {
        Init();
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

    void GLTexture::Init() {
        WZ_CORE_TRACE("Initializing GL Texture...");

        GL_CALL(glGenTextures(1, &m_textureId));

        GL_CALL(glBindTexture(GL_TEXTURE_2D, m_textureId));

        WZ_CORE_TRACE("Generated GL texture with id '{0}'", m_textureId);

        GLenum _wrapMode = DEFAULT_WRAP_MODE;
        GLenum _minFilterMode = DEFAULT_MIN_FILTER_MODE;
        GLenum _magFilterMode = DEFAULT_MAG_FILTER_MODE;

        if (m_flags.GetBit(WZ_WRAP_MODE_CLAMP_TO_EDGE)) {
            _wrapMode = GL_CLAMP_TO_EDGE;
        } else if (m_flags.GetBit(WZ_WRAP_MODE_MIRRORED_REPEAT)) {
            _wrapMode = GL_MIRRORED_REPEAT;
        } else if (m_flags.GetBit(WZ_WRAP_MODE_REPEAT)) {
            _wrapMode = GL_REPEAT;
        }

        if (m_flags.GetBit(WZ_MIN_FILTER_MODE_NEAREST)) {
            _minFilterMode = GL_NEAREST;
        } else if (m_flags.GetBit(WZ_MIN_FILTER_MODE_LINEAR)) {
            _minFilterMode = GL_LINEAR;
        } else if (m_flags.GetBit(WZ_MIN_FILTER_MODE_NEAREST_MIPMAP_NEAREST)) {
            _minFilterMode = GL_NEAREST_MIPMAP_NEAREST;
        } else if (m_flags.GetBit(WZ_MIN_FILTER_MODE_LINEAR_MIPMAP_NEAREST)) {
            _minFilterMode = GL_LINEAR_MIPMAP_NEAREST;
        } else if (m_flags.GetBit(WZ_MIN_FILTER_MODE_NEAREST_MIPMAP_LINEAR)) {
            _minFilterMode = GL_NEAREST_MIPMAP_LINEAR;
        } else if (m_flags.GetBit(WZ_MIN_FILTER_MODE_LINEAR_MIPMAP_LINEAR)) {
            _minFilterMode = GL_LINEAR_MIPMAP_LINEAR;
        }

        if (m_flags.GetBit(WZ_MAG_FILTER_NEAREST)) {
            _magFilterMode = GL_NEAREST;
        } else if (m_flags.GetBit(WZ_MAG_FILTER_LINEAR)) {
            _magFilterMode = GL_LINEAR;
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

        GLenum _internalFormat = GL_RGB;

        switch (m_channels) {
            case 1: _internalFormat = GL_RED; break;
            case 2: _internalFormat = GL_RG; break;
            case 3: _internalFormat = GL_RGB; break;
            case 4: _internalFormat = GL_RGBA; break;
        }

        WZ_CORE_TRACE("Creating GL tex2d depending on given image type in flags");
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, m_width, m_height,
                             0, GL_RGBA, GL_UNSIGNED_BYTE, m_data));

        WZ_CORE_TRACE("Generating mitmaps");
        if (!m_flags.GetBit(WZ_DONT_GENERATE_MIPMAP)) {
            GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
        }


        m_isValid = true;
        WZ_CORE_INFO("Successfully Initialized GL Texture and assigned id '{0}'", m_textureId);
    }
}
