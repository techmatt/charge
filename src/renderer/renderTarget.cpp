﻿
#include "main.h"

void RenderTarget::init(Renderer &renderer, const vec2i &dimensions)
{
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

    glBindFramebuffer(GL_TEXTURE_2D, _frameBuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

    glBindFramebuffer(GL_TEXTURE_2D, 0);
    
    //to render:
    //glBindFramebuffer
    //glBindFramebuffer(null)
}

void RenderTarget::bindAsRenderTarget()
{
    glBindFramebuffer(GL_TEXTURE_2D, _frameBuffer);
}

void RenderTarget::unbindRenderTarget()
{
    glBindFramebuffer(GL_TEXTURE_2D, 0);
}

void RenderTarget::bindAsTexture()
{
    glBindTexture(GL_TEXTURE_2D, _texture);
}

Bitmap RenderTarget::getImage()
{
    Bitmap result(_dimensions.x, _dimensions.y);

    glBindFramebuffer(GL_TEXTURE_2D, _frameBuffer);
    glReadPixels(0, 0, _dimensions.x, _dimensions.y, GL_RGBA, GL_UNSIGNED_BYTE, result.data());
    glBindFramebuffer(GL_TEXTURE_2D, 0);

    return result;
}
