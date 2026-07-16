#include "render/render_target.h"
#include <glad/glad.h>

static constexpr GLsizei FBO_MSAA_SAMPLES = 4;

void SwapchainRenderTarget::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool TextureRenderTarget::init(int width, int height) {
    if (width <= 0 || height <= 0)
        return false;

    if (width == m_width && height == m_height && fbo_id != 0)
        return true;

    destroy();

    glGenFramebuffers(1, &fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

    glGenRenderbuffers(1, &fbo_color_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, fbo_color_rbo_id);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, FBO_MSAA_SAMPLES, GL_SRGB8_ALPHA8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, fbo_color_rbo_id);

    glGenRenderbuffers(1, &fbo_depth_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, fbo_depth_rbo_id);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, FBO_MSAA_SAMPLES, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo_depth_rbo_id);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    glGenFramebuffers(1, &resolve_fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, resolve_fbo_id);

    glGenTextures(1, &resolve_color_tex_id);
    glBindTexture(GL_TEXTURE_2D, resolve_color_tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resolve_color_tex_id, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_width = width;
    m_height = height;
    return true;
}

void TextureRenderTarget::destroy() {
    if (fbo_id) { glDeleteFramebuffers(1, &fbo_id); fbo_id = 0; }
    if (fbo_color_rbo_id) { glDeleteRenderbuffers(1, &fbo_color_rbo_id); fbo_color_rbo_id = 0; }
    if (fbo_depth_rbo_id) { glDeleteRenderbuffers(1, &fbo_depth_rbo_id); fbo_depth_rbo_id = 0; }
    if (resolve_fbo_id) { glDeleteFramebuffers(1, &resolve_fbo_id); resolve_fbo_id = 0; }
    if (resolve_color_tex_id) { glDeleteTextures(1, &resolve_color_tex_id); resolve_color_tex_id = 0; }
    m_width = 0;
    m_height = 0;
}

void TextureRenderTarget::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
}

void TextureRenderTarget::resolve() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolve_fbo_id);
    glBlitFramebuffer(
        0, 0, m_width, m_height,
        0, 0, m_width, m_height,
        GL_COLOR_BUFFER_BIT, GL_NEAREST
    );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TextureRenderTarget::resize(int width, int height) {
    init(width, height);
}
