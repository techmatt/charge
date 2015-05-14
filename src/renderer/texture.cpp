
#include "main.h"

void Texture::load(RendererSDL &renderer, const string &filename)
{
    _bmp = LodePNG::load(filename);
    LodePNG::save(_bmp, "test.png");
    initSDL(renderer);
}

void Texture::initSDL(RendererSDL &renderer)
{
    _SDLTexture = nullptr;
    
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

    //SDL_FreeSurface(loadedImage);
}
