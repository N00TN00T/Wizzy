#include "wzpch.h"

#include <glad/glad.h>

#include "Wizzy/Renderer/RenderTarget.h"
#include "Wizzy/platform/OpenGL/GLRenderTarget.h"
#include "Wizzy/platform/OpenGL/GLErrorHandling.h"

namespace Wizzy {

    GLRenderTarget::GLRenderTarget(u32 width, u32 height)
        : m_width(width), m_height(height) {
        WZ_CORE_TRACE("Initializing GL RenderTarget using framebuffer");
        WZ_CORE_TRACE("Width: {0}, Height: {1}", width, height);
        WZ_CORE_ASSERT(width > 0 && height > 0, "Invalid size dimensions for RenderTarget");
        GL_CALL(glGenFramebuffers(1, &m_frameBufferId));
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferId));

        WZ_CORE_TRACE("Creating empty internal GL texture");
        m_textureId = CreateTexture(width, height);

        WZ_CORE_TRACE("Creating render buffer");
        m_renderBufferId = CreateRenderBuffer(width, height);

        WZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "GL Frame buffer initialization failed");

        this->Unbind();

        WZ_CORE_INFO("Successfully intitialized GL RenderTarget. Width: {0}, Height: {1}, FBO: {2}, RBO: {3}, textureId: {4}", width, height, m_frameBufferId, m_renderBufferId, m_textureId);
    }
    GLRenderTarget::~GLRenderTarget() {
        WZ_CORE_TRACE("Destructing GL RenderTarget");
        GL_CALL(glDeleteFramebuffers(1, &m_frameBufferId));
        WZ_CORE_TRACE("Destructed framebuffer, destructing renderbuffer");
        GL_CALL(glDeleteRenderbuffers(1, &m_renderBufferId));
        WZ_CORE_TRACE("Destructed renderbuffer, destructing texture");
        GL_CALL(glDeleteTextures(1, &m_textureId));
    }

    void GLRenderTarget::Bind() const {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferId));
    }
    void GLRenderTarget::Unbind() const {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void GLRenderTarget::SetSize(u32 width, u32 height) {

        WZ_CORE_TRACE("Changing size on GL RenderTarget (framebuffer)");

        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferId));

        SetTexture(m_textureId, width, height);

        SetRenderBuffer(m_renderBufferId, width, height);

        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

        WZ_CORE_TRACE("Changed size from { {0}, {1} } to { {2}, {3} }", m_width, m_height, width, height);

        m_width = width;
        m_height = height;
    }

    u32 GLRenderTarget::CreateTexture(u32 width, u32 height) {
        u32 _textureId;
        GL_CALL(glGenTextures(1, &_textureId));

        SetTexture(_textureId, width, height);

        return _textureId;
    }

    u32 GLRenderTarget::CreateRenderBuffer(u32 width, u32 height) {
        u32 _renderBufferId;
        GL_CALL(glGenRenderbuffers(1, &_renderBufferId));

        SetRenderBuffer(_renderBufferId, width, height);

        return _renderBufferId;
    }

    void GLRenderTarget::SetTexture(u32 textureId, u32 width, u32 height) {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, textureId));

        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, NULL));

        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, textureId, 0));

        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }
    void GLRenderTarget::SetRenderBuffer(u32 renderBufferId, u32 width, u32 height) {
        GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId));

        GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                                      width, height));
        GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                          GL_DEPTH_STENCIL_ATTACHMENT,
                                          GL_RENDERBUFFER,
                                          renderBufferId));

        GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    }
}
