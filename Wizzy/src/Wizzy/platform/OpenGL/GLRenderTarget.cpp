#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/Exceptions/WizzyExceptions.h"
#include "Wizzy/platform/OpenGL/GLRenderTarget.h"
#include "Wizzy/platform/OpenGL/GLErrorHandling.h"
#include "Wizzy/Utilities/ImageImporter.h"

namespace Wizzy {

    GLRenderTarget::GLRenderTarget(s32 width, s32 height, s32 channels)
        : m_width(width), m_height(height), m_channels(channels), RenderTarget(PropertyTable()) {
        Init();
    }
    GLRenderTarget::GLRenderTarget(const ResData& encoded, const PropertyTable& props)
        : RenderTarget(props)
    {
        int32 c;
        Image::set_vertical_flip_on_load(true);
        auto decoded = Image::decode_image(encoded.data(), encoded.size(), m_width, m_height, c, 4);
        if (decoded)
        {
            Init(decoded);
            Image::free_image(decoded);
        }
        else
        {
            WZ_THROW(Exception, "Failed loading render target: '" + string(Image::get_failure_reason()) + "'");
            return;
        }

        Init((byte*)&decoded[0]);
    }
    GLRenderTarget::~GLRenderTarget() {
        WZ_CORE_TRACE("Destructed renderbuffer, destructing texture");
        GL_CALL(glDeleteTextures(1, &m_textureId));
        WZ_CORE_TRACE("Destructed framebuffer");
        GL_CALL(glDeleteFramebuffers(1, &m_frameBufferId));
    }

    void GLRenderTarget::Bind() const {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferId));
    }
    void GLRenderTarget::Unbind() const {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void GLRenderTarget::BindTexture(u32 location) const
    {
        GL_CALL(glActiveTexture(GL_TEXTURE0 + location));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, m_textureId));
    }

    void GLRenderTarget::UnbindTexture() const
    {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }

    void GLRenderTarget::SetSize(s32 width, s32 height) {

        WZ_CORE_TRACE("Changing size on GL RenderTarget (framebuffer)");

        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferId));

        SetTexture(m_textureId, width, height, m_channels);

        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

        WZ_CORE_TRACE("Changed size from { {0}, {1} } to { {2}, {3} }", m_width, m_height, width, height);

        m_width = width;
        m_height = height;
    }

    u32 GLRenderTarget::CreateTexture(s32 width, s32 height, s32 channels, byte* data) {
        u32 _textureId;
        GL_CALL(glGenTextures(1, &_textureId));

        SetTexture(_textureId, width, height, channels, data);

        return _textureId;
    }

    void GLRenderTarget::SetTexture(u32 textureId, s32 width, s32 height, s32 channels, byte* data) {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, textureId));

        GLenum fmt = TextureChannelsToAPIFormat(channels);

        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt,
                     GL_UNSIGNED_BYTE, data));

        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, textureId, 0));

        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }
    void GLRenderTarget::Init(byte* data)
    {
        WZ_CORE_TRACE("Initializing GL RenderTarget using framebuffer");
        WZ_CORE_TRACE("Width: {0}, Height: {1}", m_width, m_height);
        WZ_CORE_ASSERT(m_width > 0 && m_height > 0, "Invalid size dimensions for RenderTarget");
        GL_CALL(glGenFramebuffers(1, &m_frameBufferId));
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferId));

        WZ_CORE_TRACE("Creating empty internal GL texture");
        m_textureId = CreateTexture(m_width, m_height, m_channels, data);

        WZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "GL Frame buffer initialization failed");

        this->Unbind();

        WZ_CORE_TRACE("GL Render Target initialized (Width: {0}, Height: {1}, FBO: {2}, textureId: {4})", m_width, m_height, m_frameBufferId, m_textureId);
    }
    ResData GLRenderTarget::Serialize() const
    {
        WZ_CORE_TRACE("Serializing GL RenderTarget");
        byte* data = (byte*)malloc(m_width * (size_t)m_height * 4ULL);

        this->Bind();
        WZ_CORE_TRACE("Reading pixels");
        GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
        GL_CALL(glReadPixels(0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, data));

        int32 len;
        Image::set_vertical_flip_on_write(true);
        byte* serializedBytes = Image::encode_png(data, m_width, m_height, 4, len);
        ResData serialized(serializedBytes, serializedBytes + len);

        delete serializedBytes;
        free(data);

        this->Unbind();

        return serialized;
    }
}
