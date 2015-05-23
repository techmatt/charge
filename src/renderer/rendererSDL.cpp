
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

void RendererSDL::render(Texture &tex, const rect2f &destinationRect, CoordinateFrame &frame)
{
	rect2f transformedDestinationRect = frame.toContainer(destinationRect);
	SDL_Rect dst;
	dst.x = (int) (transformedDestinationRect.min().x);
	dst.y = (int) (transformedDestinationRect.min().y);
	dst.w = (int) (transformedDestinationRect.max().x)-dst.x;
	dst.h = (int) (transformedDestinationRect.max().y)-dst.y;

	SDL_RenderCopy(_renderer, tex.SDL(), NULL, &dst);
}

void RendererSDL::render(Texture &tex, const rect2i &destinationRect, CoordinateFrame &frame)
{
	render(tex, rect2f(destinationRect.min(), destinationRect.max()),frame);
}

void RendererSDL::render(Texture &tex, const rect2f &destinationRect, float angle, CoordinateFrame &frame)
{
	rect2f transformedDestinationRect = frame.toContainer(destinationRect);
	SDL_Rect dst;
	dst.x = (int)transformedDestinationRect.min().x;
	dst.y = (int)transformedDestinationRect.min().y;
	dst.w = (int)transformedDestinationRect.extentX();
	dst.h = (int)transformedDestinationRect.extentY();

	SDL_RenderCopyEx(_renderer, tex.SDL(), NULL, &dst, angle, NULL, SDL_FLIP_NONE);
}
void RendererSDL::render(Texture &tex, const rect2i &destinationRect, float angle, CoordinateFrame &frame)
{
	render(tex, rect2f(destinationRect.min(), destinationRect.max()),angle, frame);
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
	return vec2f(params().canonicalDims);
}

CoordinateFrame RendererSDL::getWindowCoordinateFrame()
{
	vec2i result;
	SDL_GetWindowSize(_window, &result.x, &result.y);  // the actual window size

	float height = (float)result.x;
	float width = (float)result.y;

	vec2i canonical = RendererSDL::getWindowSize();   // the canonical window size

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

	CoordinateFrame out = CoordinateFrame(start, end, vec2f(canonical));

	return out;
}