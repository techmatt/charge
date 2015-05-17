
#include "main.h"

void RendererSDL::init(SDL_Window *window)
{
    _window = window;
    _renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (_renderer == nullptr)
    {
        SDL::logError("CreateRenderer");
        SDL_Quit();
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
}

void RendererSDL::render(Texture &tex, int x, int y)
{
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;

    SDL_QueryTexture(tex.SDL(), NULL, NULL, &dst.w, &dst.h);

    SDL_RenderCopy(_renderer, tex.SDL(), NULL, &dst);
}

void RendererSDL::render(Texture &tex, const rect2f &destinationRect)
{
	render(tex, rect2i(destinationRect.min(), destinationRect.max()));
}

void RendererSDL::render(Texture &tex, const rect2i &destinationRect)
{
    SDL_Rect dst;
    dst.x = destinationRect.min().x;
    dst.y = destinationRect.min().y;
    dst.w = destinationRect.extentX();
    dst.h = destinationRect.extentY();

    SDL_RenderCopy(_renderer, tex.SDL(), NULL, &dst);
}

void RendererSDL::present()
{
    SDL_RenderPresent(_renderer);
}

void RendererSDL::setRenderTarget(Texture &target)
{
    SDL_SetRenderTarget(_renderer, target.SDL());
}

void RendererSDL::setDefaultRenderTarget()
{
    SDL_SetRenderTarget(_renderer, NULL);
}

//vec2i RendererSDL::getRenderTargetDimensions()
//{
//    SDL_Texture* target = SDL_GetRenderTarget(_renderer);
//
//    int width, height;
//    Uint32 format;
//    int access;
//    SDL_QueryTexture(target, &format, &access, &width, &height);
//
//    return vec2i(width, height);
//}

vec2i RendererSDL::getWindowSize()
{
    vec2i result;
    SDL_GetWindowSize(_window, &result.x, &result.y);
    return result;
}
