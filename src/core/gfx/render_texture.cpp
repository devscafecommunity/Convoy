#include "render_texture.h"
#include <glad/glad.h>

namespace convoy {

void RenderTexture::create(int width, int height) {
    destroy();
    width_  = width;
    height_ = height;

    glGenFramebuffers(1, &fbo_id_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);

    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, texture_id_, 0);

    glGenRenderbuffers(1, &rbo_id_);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_id_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rbo_id_);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderTexture::resize(int width, int height) {
    if (width == width_ && height == height_) return;
    create(width, height);
}

void RenderTexture::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);
    glViewport(0, 0, width_, height_);
}

void RenderTexture::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTexture::destroy() {
    if (rbo_id_)     { glDeleteRenderbuffers(1, &rbo_id_);    rbo_id_     = 0; }
    if (texture_id_) { glDeleteTextures(1, &texture_id_);     texture_id_ = 0; }
    if (fbo_id_)     { glDeleteFramebuffers(1, &fbo_id_);     fbo_id_     = 0; }
    width_ = height_ = 0;
}

}
