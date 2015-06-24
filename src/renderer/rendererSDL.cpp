
#include "main.h"

void RendererSDL::init(SDL_Window *window)
{
	_window = window;
    _renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
	if (_renderer == nullptr)
	{
        cout << "Error creating SDL renderer" << endl;
		SDL_Quit();
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
}

void RendererSDL::render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color)
{
	SDL_Rect dst;
    dst.x = (int)(destinationRect.min().x);
    dst.y = (int)(destinationRect.min().y);
    dst.w = (int)(destinationRect.max().x) - dst.x;
    dst.h = (int)(destinationRect.max().y) - dst.y;

    SDL_SetTextureColorMod(tex.SDL(), util::boundToByte(color.r * 255.0f), util::boundToByte(color.g * 255.0f), util::boundToByte(color.b * 255.0f));
	SDL_RenderCopy(_renderer, tex.SDL(), NULL, &dst);
}

void RendererSDL::render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color)
{
	SDL_Rect dst;
    dst.x = (int)(destinationRect.min().x);
    dst.y = (int)(destinationRect.min().y);
    dst.w = (int)(destinationRect.max().x) - dst.x;
    dst.h = (int)(destinationRect.max().y) - dst.y;

    SDL_RenderCopyEx(_renderer, tex.SDL(), NULL, &dst, rotation, NULL, SDL_FLIP_NONE);
}

void RendererSDL::clear()
{
    SDL_RenderClear(_renderer);
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

vec2i RendererSDL::getWindowSize()
{
    vec2i result;
    SDL_GetWindowSize(_window, &result.x, &result.y);
	return result;
}

CoordinateFrame RendererSDL::getWindowCoordinateFrame()
{
    const vec2f windowSize = getWindowSize();

    float height = (float)windowSize.x;
    float width = (float)windowSize.y;

	vec2i canonical = GameUtil::getCanonicalSize();

	float aspectRatio = ((float) canonical.y) / (float) canonical.x;

	vec2f start;
	vec2f end;

	if (width > aspectRatio * height)
	{
		start = vec2f(0.0f,(width - height*aspectRatio) / 2.0f);
		end = vec2f(height, (width + height*aspectRatio) / 2.0f);
	}
	else
	{
		start = vec2f((height - width/aspectRatio) / 2.0f,0.0f);
		end = vec2f((height + width / aspectRatio) / 2.0f, width);
	}

    return CoordinateFrame(start, end, vec2f(canonical));
}

void RendererSDL::bindMainRenderTarget()
{
    SDL_SetRenderTarget(SDL(), nullptr);
}