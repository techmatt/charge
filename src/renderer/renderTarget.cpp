
#include "main.h"

void RenderTarget::init(RendererOpenGL &renderer, const vec2i &dimensions)
{
    releaseMemory();
    _dimensions = dimensions;

    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);

    const GLsizei width = dimensions.x;
    const GLsizei height = dimensions.y;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &_frameBuffer);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _frameBuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
    bindAsRenderTarget();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderer.bindMainRenderTarget();
}

void RenderTarget::bindAsRenderTarget()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _frameBuffer);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);
}

void RenderTarget::unbindRenderTarget()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void RenderTarget::bindAsTexture()
{
    glBindTexture(GL_TEXTURE_2D, _texture);
}

Bitmap RenderTarget::getImage()
{
    Bitmap result(_dimensions.x, _dimensions.y);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, _frameBuffer);
    glReadPixels(0, 0, _dimensions.x, _dimensions.y, GL_RGBA, GL_UNSIGNED_BYTE, result.data());
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    return result;
}
