
#include "main.h"

void Texture::load(RendererSDL &renderer, const string &filename)
{
    _renderTarget = false;
    _bmp = LodePNG::load(filename);
    LodePNG::save(_bmp, "test.png");
    initSDL(renderer);
}

void Texture::loadRenderTarget(int width, int height)
{
    _bmp.allocate(width, height);
    _renderTarget = true;
}

void Texture::initSDL(RendererSDL &renderer)
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
        _SDLSurface = SDL_CreateRGBSurfaceFrom(_bmp.data(), (int)_bmp.width(), (int)_bmp.height(), 32, (int)_bmp.width() * sizeof(RGBColor), 0x0000FF, 0x00FF00, 0xFF0000, 0);
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
