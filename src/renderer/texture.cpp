
#include "main.h"

void Texture::load(Renderer &renderer, const string &filename)
{
    _renderTarget = false;
    _bmp = LodePNG::load(filename);

    if (renderer.type() == RendererTypeSDL)
        initSDL(renderer);

    if (renderer.type() == RendererTypeOpenGL)
        initOpenGL();
}

void Texture::load(Renderer &renderer, const Bitmap &bmp)
{
	_renderTarget = false;
	_bmp = bmp;

    if (renderer.type() == RendererTypeSDL)
        initSDL(renderer);

    if (renderer.type() == RendererTypeOpenGL)
        initOpenGL();
}

void Texture::loadRenderTarget(int width, int height)
{
    _bmp.allocate(width, height);
    _renderTarget = true;
}

void Texture::initSDL(Renderer &renderer)
{
    releaseSDLMemrory();

    _SDLTexture = nullptr;
    
    if (_renderTarget)
    {
        //SDL_PIXELFORMAT_ARGB8888 seems to be the default texture for windows
        SDL_CreateTexture(renderer.SDL(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, (int)_bmp.width(), (int)_bmp.height());
    }
    else
    {
		_SDLSurface = SDL_CreateRGBSurfaceFrom(_bmp.data(), (int)_bmp.width(), (int)_bmp.height(), 32, (int)_bmp.width() * sizeof(RGBColor), 0x0000FF, 0x00FF00, 0xFF0000, 0xFF000000);
        if (_SDLSurface == nullptr)
        {
            SDL::logError("SDL_CreateRGBSurfaceFrom");
            return;
        }

        _SDLTexture = SDL_CreateTextureFromSurface(renderer.SDL(), _SDLSurface);

        if (_SDLTexture == nullptr)
        {
            SDL::logError("SDL_CreateTextureFromSurface");
            return;
        }
    }

    //SDL_FreeSurface(loadedImage);
}

void Texture::initOpenGL()
{
    glGenTextures(1, &_OpenGLTexture);
    glBindTexture(GL_TEXTURE_2D, _OpenGLTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)_bmp.dimX(), (GLsizei)_bmp.dimY(), 0, GL_RGBA, GL_UNSIGNED_BYTE, _bmp.data());
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::bindOpenGL()
{
    //SDL_GL_BindTexture(_SDLTexture, nullptr, nullptr);
    glBindTexture(GL_TEXTURE_2D, _OpenGLTexture);
}