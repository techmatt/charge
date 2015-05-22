
#include "main.h"

void RendererSDL::init(SDL_Window *window)
{
    _context = SDL_GL_CreateContext(window);
    if (_context == nullptr)
    {
        SDL::logError("CreateContext");
        SDL_Quit();
    }

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

void RendererSDL::render(Texture &tex, const rect2i &destinationRect, float angle)
{
	SDL_Rect dst;
	dst.x = destinationRect.min().x;
	dst.y = destinationRect.min().y;
	dst.w = destinationRect.extentX();
	dst.h = destinationRect.extentY();

	SDL_RenderCopyEx(_renderer, tex.SDL(), NULL, &dst, angle, NULL, SDL_FLIP_NONE);
}

void RendererSDL::present()
{
	SDL_RenderPresent(_renderer);
    SDL_GL_SwapWindow(_window);
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

	double width = (double)result.x;
	double height = (double)result.y;

	if (width > constants::screenAspectRatio * height)
		result.x = (int) (height * constants::screenAspectRatio);
	else
		result.y = (int) floor(width / constants::screenAspectRatio);
	
	return result;
}

vec2i RendererSDL::getWindowStart()
{
	vec2i result;
	SDL_GetWindowSize(_window, &result.x, &result.y);

	double width = (double)result.x;
	double height = (double)result.y;

	int dim;

	if (width > constants::screenAspectRatio * height)
	{
		dim = (int)floor(height * constants::screenAspectRatio);
        result.x = (int)floor((width - ((double)dim)) / 2.);
		result.y = 0;
	}

	else
	{
        dim = (int)floor(width / constants::screenAspectRatio);
        result.x = (int)floor((width - ((double)dim)) / 2.);
		result.y = 0;
	}

	return result;
}