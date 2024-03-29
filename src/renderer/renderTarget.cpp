﻿
#include "main.h"

void RenderTarget::init(Renderer &renderer, const vec2i &dimensions)
{
    _renderer = &renderer;
    releaseMemory();
    _dimensions = dimensions;

    if (renderer.type() == RendererType::OpenGL)
    {
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
        checkGLError();

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _frameBuffer);
        checkGLError();

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);
        checkGLError();

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        checkGLError();

        bindAsRenderTarget();
        checkGLError();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        checkGLError();

        renderer.bindMainRenderTarget();
    }
    else
    {
        _SDLTexture = SDL_CreateTexture(renderer.SDL(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, dimensions.x, dimensions.y);
    }
}

void RenderTarget::clear(const vec4f &color)
{
    bindAsRenderTarget();
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
    unbindRenderTarget();
}

void RenderTarget::bindAsRenderTarget()
{
    if (_renderer == nullptr) return;

    if (_renderer->type() == RendererType::SDL)
    {
        SDL_SetRenderTarget(_renderer->SDL(), _SDLTexture);
    }
    else
    {
        checkGLError();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _frameBuffer);
        checkGLError();
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);
    }
}

void RenderTarget::unbindRenderTarget()
{
    if (_renderer->type() == RendererType::SDL)
    {
        SDL_SetRenderTarget(_renderer->SDL(), nullptr);
    }
    else
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
}

void RenderTarget::bindAsTexture()
{
    if (_texture == 0) return;

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
