
#include "main.h"

void RendererSDL::init(SDL_Window *window)
{
    _renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (_renderer == nullptr)
    {
        SDL::logError("CreateRenderer");
        SDL_Quit();
    }
}

void RendererSDL::render(Texture &tex, int x, int y)
{
    //Setup the destination rectangle to be at the position we want
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;

    //Query the texture to get its width and height to use
    SDL_QueryTexture(tex.SDL(), NULL, NULL, &dst.w, &dst.h);

    SDL_RenderCopy(_renderer, tex.SDL(), NULL, &dst);
}
